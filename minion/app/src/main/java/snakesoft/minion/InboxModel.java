package snakesoft.minion;

import com.google.protobuf.InvalidProtocolBufferException;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.util.Vector;

import inbox.Inbox;
import inbox.Inbox.InboxItems;

import static snakesoft.minion.StatusOfChange.Type.Edit;

class StatusOfChange implements java.io.Serializable
{
    enum Type
    {
        None,
        Add,
        Delete,
        Edit
    }

    public Type type;

    public StatusOfChange()
    {
        type = Type.None;
    }
}

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
                if(mItemsChanges.get(i).type == Edit)
                {
                    mProxy.editItem(mItems.getItems(i));
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

    public InboxItems getItems()
    {
        return mItems;
    }

    public void modifyItem(Inbox.InboxItemInfo item)
    {
        for(int i = 0; i < mItems.getItemsCount(); ++i)
        {
            if(mItems.getItems(i).getId().getGuid().equals(item.getId().getGuid()))
            {
                mItems = InboxItems.newBuilder(mItems).setItems(i, item).build();
                StatusOfChange ch = new StatusOfChange();
                ch.type = Edit;
                mItemsChanges.set(i, ch);
                break;
            }
        }

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

    private InboxServiceProxy mProxy;
    private InboxItems mItems;
    private LocalDatabase mLocalDb;
    private Vector<StatusOfChange> mItemsChanges;
}
