package snakesoft.minion;

import com.google.protobuf.InvalidProtocolBufferException;

import java.util.List;

import common.Common;
import inbox.Inbox;

/*
   rpc GetInbox (common.EmptyMessage) returns (InboxItems);
   rpc DeleteItem (common.UniqueId) returns (common.OperationResultMessage);
   rpc EditItem (InboxItemInfo) returns (common.OperationResultMessage);
   rpc AddItem (InboxItemInfo) returns (common.UniqueId);
 */
public class InboxServiceProxy
{
    public InboxServiceProxy(MateriaConnection materiaConnection)
    {
        mMateriaConnection = materiaConnection;
    }

    public Inbox.InboxItems getInbox() throws InvalidProtocolBufferException, MateriaUnreachableException {
        return Inbox.InboxItems.parseFrom(mMateriaConnection.sendMessage(
                Common.EmptyMessage.newBuilder().build().toByteString(),
                "InboxService",
                "GetInbox"
        ));
    }

    public void deleteItem(Common.UniqueId id) throws InvalidProtocolBufferException, MateriaUnreachableException {
        mMateriaConnection.sendMessage(
                id.toByteString(),
                "InboxService",
                "DeleteItem"
        );
    }

    public Common.UniqueId addItem(Inbox.InboxItemInfo item) throws InvalidProtocolBufferException, MateriaUnreachableException {
        return Common.UniqueId.parseFrom(mMateriaConnection.sendMessage(
                item.toByteString(),
                "InboxService",
                "AddItem"
        ));
    }

    public boolean editItem(Inbox.InboxItemInfo item) throws InvalidProtocolBufferException, MateriaUnreachableException {
        return Common.OperationResultMessage.parseFrom(mMateriaConnection.sendMessage(
                item.toByteString(),
                "InboxService",
                "EditItem"
        )).getSuccess();
    }

    private MateriaConnection mMateriaConnection;
}
