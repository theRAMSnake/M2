package snakesoft.minion;

import com.google.protobuf.InvalidProtocolBufferException;
import com.google.protobuf.Mixin;
import com.unnamed.b.atv.model.TreeNode;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.util.HashMap;
import java.util.Map;
import java.util.Vector;

import actions.Actions;
import common.Common;

/**
 * Created by snake on 5/14/17.
 */

enum ActionsTreeType
{
    Normal,
    Backlog
}

public class ActionsModel
{
    public ActionsModel(ActionsServiceProxy actionsServiceProxy)
    {
        mProxy = actionsServiceProxy;
        mItemsChanges = new Vector<>();
    }

    public void sync() throws MateriaUnreachableException {
        try
        {
            Map<String, String> virtualToRealIdMap = new HashMap<>();

            for(int i = 0; i < mItemsChanges.size(); ++i)
            {
                if(mItemsChanges.get(i).type == StatusOfChange.Type.Edit)
                {
                    mProxy.editElement(mItems.getList(i));
                }
                else if(mItemsChanges.get(i).type == StatusOfChange.Type.Delete)
                {
                    mProxy.deleteElement(mItems.getList(i).getId());
                }
                else if(mItemsChanges.get(i).type == StatusOfChange.Type.Add)
                {
                    Actions.ActionInfo curItem = mItems.getList(i);

                    if(virtualToRealIdMap.containsKey(curItem.getParentId().getGuid()))
                    {
                        String realId = virtualToRealIdMap.get(curItem.getParentId().getGuid());
                        curItem = Actions.ActionInfo.newBuilder(curItem)
                                .setParentId(Common.UniqueId.newBuilder().setGuid(realId))
                                .build();
                    }

                    String oldId = curItem.getId().getGuid();
                    String newId = mProxy.addElement(curItem).getGuid();

                    virtualToRealIdMap.put(oldId, newId);
                }
            }

            mItemsChanges.clear();

            Actions.ActionsList items = mProxy.getParentlessElements();
            mItems = Actions.ActionsList.newBuilder(items).build();
            for(int i = 0; i < items.getListCount(); ++i)
            {
                //if(items.getList(i).getType() == Actions.ActionType.Group)
                {
                    mItems = merge(mItems, fetchRecursive(items.getList(i).getId()));
                }
            }

            for(int i = 0; i < mItems.getListCount(); ++i)
            {
                mItemsChanges.addElement(new StatusOfChange());
            }
        }
        catch (InvalidProtocolBufferException ex)
        {

        }

        saveState();
    }

    public void loadState(LocalDatabase localDb) throws Exception {
        mLocalDb = localDb;
        try
        {
            mItems = Actions.ActionsList.parseFrom(localDb.get("ActionItems"));

            ByteArrayInputStream byteStream = new ByteArrayInputStream(localDb.get("ActionItemsStatus"));
            int next = byteStream.read();
            while(next != -1)
            {
                StatusOfChange ch = new StatusOfChange();
                ch.type = StatusOfChange.Type.values()[ next];
                mItemsChanges.add(ch);

                next = byteStream.read();
            }

            assert mItemsChanges.size() == mItems.getListCount();

            for(int i = 0; i < mItems.getListCount(); ++i)
            {
                if(mItems.getList(i).getId().getGuid().length() < 10)
                {
                    int curVirtualId = Integer.parseInt(mItems.getList(i).getId().getGuid());
                    if(curVirtualId > mLastVirtualId)
                    {
                        mLastVirtualId = curVirtualId;
                    }
                }
            }
        }
        catch (InvalidProtocolBufferException ex)
        {

        }
        catch (NullPointerException ex)
        {
            if(mItems == null)
            {
                mItems = Actions.ActionsList.newBuilder().build();
            }

            if(mItems.getListCount() != mItemsChanges.size())
            {
                for(int i = 0; i < mItems.getListCount(); ++i)
                {
                    mItemsChanges.addElement(new StatusOfChange());
                }
            }
        }

        //db check
        if(mItems.getListCount() != mItemsChanges.size())
        {
            throw new Exception("Inconsistent DB!");
        }
    }

    public void saveState()
    {
        mLocalDb.put("ActionItems", mItems.toByteArray());
        ByteArrayOutputStream bos = new ByteArrayOutputStream();

        for(StatusOfChange x : mItemsChanges)
        {
            bos.write(x.type.ordinal());
        }

        mLocalDb.put("ActionItemsStatus", bos.toByteArray());
    }

    private Actions.ActionsList fetchRecursive(Common.UniqueId id) throws InvalidProtocolBufferException, MateriaUnreachableException
    {
        Actions.ActionsList items = mProxy.getChildren(id);
        Actions.ActionsList result = Actions.ActionsList.newBuilder(items).build();
        for(int i = 0; i < items.getListCount(); ++i)
        {
            //if(items.getList(i).getType() == Actions.ActionType.Group)
            {
                result = merge(result, fetchRecursive(items.getList(i).getId()));
            }
        }

        return result;
    }

    private Actions.ActionsList merge(Actions.ActionsList a, Actions.ActionsList b)
    {
        return Actions.ActionsList.newBuilder(a).mergeFrom(b).build();
    }

    public void fillTreeRoot(TreeNode root, ActionsTreeType typeOfTree)
    {
        //Here we assume that children always has index greater that its parent.
        Map<String, TreeNode> idToIndexMap = new HashMap<>();

        root.addChild(new TreeNode(null));

        for(int i = 0; i < mItems.getListCount(); ++i)
        {
            Actions.ActionInfo curItem = mItems.getList(i);

            if(mItemsChanges.get(i).type != StatusOfChange.Type.Delete &&
                mItemsChanges.get(i).type != StatusOfChange.Type.Junk)
            {
                TreeNode node = new TreeNode(curItem);
                idToIndexMap.put(curItem.getId().getGuid(), node);

                if (curItem.getParentId().getGuid().isEmpty())
                {
                    if(typeOfTree == ActionsTreeType.Normal && !curItem.getTitle().equals("Backlog"))
                    {
                        root.addChild(node);
                    }
                    else if(typeOfTree == ActionsTreeType.Backlog && curItem.getTitle().equals("Backlog"))
                    {
                        root.addChild(node);
                    }
                    else
                    {
                        //Filter out
                    }
                }
                else
                {
                    if(idToIndexMap.containsKey(curItem.getParentId().getGuid()))
                    {
                        idToIndexMap.get(curItem.getParentId().getGuid()).addChild(node);
                    }
                    else
                    {
                        //parent is deleted or filtered out, but items will remain until next sync
                    }
                }

                if(curItem.getType() == Actions.ActionType.Group || hasChildren(curItem.getId().getGuid()))
                {
                    node.addChild(new TreeNode(null));
                }
            }
        }
    }

    private boolean hasChildren(String guid)
    {
        for(int i = 0; i < mItems.getListCount(); ++i)
        {
            Actions.ActionInfo curItem = mItems.getList(i);
            if(curItem.getParentId().getGuid().equals(guid))
            {
                return true;
            }
        }

        return false;
    }

    public void modifyItem(Actions.ActionInfo item)
    {
        for(int i = 0; i < mItems.getListCount(); ++i)
        {
            if(mItems.getList(i).getId().getGuid().equals(item.getId().getGuid()))
            {
                mItems = Actions.ActionsList.newBuilder(mItems).setList(i, item).build();

                if(mItemsChanges.get(i).type != StatusOfChange.Type.Add)
                {
                    StatusOfChange ch = new StatusOfChange();
                    ch.type = StatusOfChange.Type.Edit;
                    mItemsChanges.set(i, ch);
                }

                break;
            }
        }

        saveState();
    }

    public void deleteItem(String guid)
    {
        for(int i = 0; i < mItems.getListCount(); ++i)
        {
            if (mItems.getList(i).getId().getGuid().equals(guid))
            {
                StatusOfChange newStatus = new StatusOfChange();

                newStatus.type = mItemsChanges.get(i).type == StatusOfChange.Type.Add
                        ? StatusOfChange.Type.Junk
                        : StatusOfChange.Type.Delete;

                mItemsChanges.set(i, newStatus);
                break;
            }
        }

        saveState();
    }

    public void addItem(Actions.ActionInfo result)
    {
        mItems = Actions.ActionsList.newBuilder(mItems).addList(result).build();

        StatusOfChange ch = new StatusOfChange();
        ch.type = StatusOfChange.Type.Add;
        mItemsChanges.add(ch);

        saveState();
    }

    public void resetChanges()
    {
        mItemsChanges.clear();
    }

    public String genId()
    {
        return Integer.toString(++mLastVirtualId);
    }

    private Actions.ActionsList mItems;
    private ActionsServiceProxy mProxy;
    private LocalDatabase mLocalDb;
    private Vector<StatusOfChange> mItemsChanges;
    private int mLastVirtualId = 0;


}
