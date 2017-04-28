package snakesoft.minion;

import com.google.protobuf.InvalidProtocolBufferException;

import inbox.Inbox;

/**
 * Created by snake on 4/28/17.
 */

public class InboxModel
{
    public InboxModel(InboxServiceProxy proxy)
    {
        mProxy = proxy;
    }

    public void sync()
    {
        try
        {
            mItems = mProxy.getInbox();
        }
        catch (InvalidProtocolBufferException ex)
        {

        }
    }

    public Inbox.InboxItems getItems()
    {
        return mItems;
    }

    private InboxServiceProxy mProxy;
    private Inbox.InboxItems mItems;
}
