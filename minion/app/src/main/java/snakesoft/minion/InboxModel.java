package snakesoft.minion;

import com.google.protobuf.InvalidProtocolBufferException;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.util.List;
import java.util.Vector;

import inbox.Inbox;
import inbox.Inbox.InboxItems;

public class InboxModel
{
    public InboxModel(InboxServiceProxy proxy)
    {
        mProxy = proxy;
        mItemsChanges = new Vector<>();
    }

    public void sync()
    {
        try
        {
            for(int i = 0; i < mItemsChanges.size(); ++i)
            {
                if(mItemsChanges.get(i).type == StatusOfChange.Type.Edit)
                {
                    mProxy.editItem(mItems.getItems(i));
                }
                else if(mItemsChanges.get(i).type == StatusOfChange.Type.Delete)
                {
                    mProxy.deleteItem(mItems.getItems(i).getId());
                }
                else if(mItemsChanges.get(i).type == StatusOfChange.Type.Add)
                {
                    mProxy.addItem(mItems.getItems(i));
                }
            }

            mItemsChanges.clear();

            mItems = mProxy.getInbox();
            for(int i = 0; i < mItems.getItemsCount(); ++i)
            {
                mItemsChanges.addElement(new StatusOfChange());
            }
        }
        catch (InvalidProtocolBufferException ex)
        {

        }

        saveState();
    }

    public void resetChanges()
    {
        mItemsChanges.clear();
    }

    public List<Inbox.InboxItemInfo> getItems()
    {
        List<Inbox.InboxItemInfo> result = new Vector<>();

        for(int i = 0; i < mItems.getItemsCount(); ++i)
        {
            if(mItemsChanges.get(i).type != StatusOfChange.Type.Delete &&
                    mItemsChanges.get(i).type != StatusOfChange.Type.Junk)
            {
                result.add(mItems.getItems(i));
            }
        }

        return result;
    }

    public void modifyItem(Inbox.InboxItemInfo item)
    {
        for(int i = 0; i < mItems.getItemsCount(); ++i)
        {
            if(mItems.getItems(i).getId().getGuid().equals(item.getId().getGuid()))
            {
                mItems = InboxItems.newBuilder(mItems).setItems(i, item).build();

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
        for(int i = 0; i < mItems.getItemsCount(); ++i)
        {
            if (mItems.getItems(i).getId().getGuid().equals(guid))
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

    public void addItem(Inbox.InboxItemInfo item)
    {
        mItems = InboxItems.newBuilder(mItems).addItems(item).build();

        StatusOfChange ch = new StatusOfChange();
        ch.type = StatusOfChange.Type.Add;
        mItemsChanges.add(ch);

        saveState();
    }

    public void loadState(LocalDatabase localDb)
    {
        mLocalDb = localDb;
        try
        {
            mItems = InboxItems.parseFrom(localDb.get("InboxItems"));
            ByteArrayInputStream byteStream = new ByteArrayInputStream(localDb.get("InboxItemsStatus"));
            int next = byteStream.read();
            while(next != -1)
            {
                StatusOfChange ch = new StatusOfChange();
                ch.type = StatusOfChange.Type.values()[ next];
                mItemsChanges.add(ch);

                next = byteStream.read();
            }

            assert mItemsChanges.size() == mItems.getItemsCount();
        }
        catch (InvalidProtocolBufferException ex)
        {

        }
        catch (NullPointerException ex)
        {
            //no data in db case
            if(mItems.getItemsCount() != mItemsChanges.size())
            {
                for(int i = 0; i < mItems.getItemsCount(); ++i)
                {
                    mItemsChanges.addElement(new StatusOfChange());
                }
            }
        }
    }

    public void saveState()
    {
        mLocalDb.put("InboxItems", mItems.toByteArray());

        ByteArrayOutputStream bos = new ByteArrayOutputStream();

        for(StatusOfChange x : mItemsChanges)
        {
            bos.write(x.type.ordinal());
        }

        mLocalDb.put("InboxItemsStatus", bos.toByteArray());
    }

    public String getNewId()
    {
        return NEW_ID;
    }

    private final String NEW_ID = "NEW_ID";
    private InboxServiceProxy mProxy;
    private InboxItems mItems;
    private LocalDatabase mLocalDb;
    private Vector<StatusOfChange> mItemsChanges;


}
