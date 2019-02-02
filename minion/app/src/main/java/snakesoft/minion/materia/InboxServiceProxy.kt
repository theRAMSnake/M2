package snakesoft.minion.materia

import com.google.protobuf.InvalidProtocolBufferException

import common.Common
import inbox.Inbox

/*
   rpc GetInbox (common.EmptyMessage) returns (InboxItems);
   rpc DeleteItem (common.UniqueId) returns (common.OperationResultMessage);
   rpc EditItem (InboxItemInfo) returns (common.OperationResultMessage);
   rpc AddItem (InboxItemInfo) returns (common.UniqueId);
 */
class InboxServiceProxy(private val mMateriaConnection: MateriaConnection) {

    val inbox: Inbox.InboxItems
        @Throws(InvalidProtocolBufferException::class, MateriaUnreachableException::class)
        get() = Inbox.InboxItems.parseFrom(mMateriaConnection.sendMessage(
                Common.EmptyMessage.newBuilder().build().toByteString(),
                "InboxService",
                "GetInbox"
        ))

    @Throws(InvalidProtocolBufferException::class, MateriaUnreachableException::class)
    fun deleteItem(id: Common.UniqueId) {
        mMateriaConnection.sendMessage(
                id.toByteString(),
                "InboxService",
                "DeleteItem"
        )
    }

    @Throws(InvalidProtocolBufferException::class, MateriaUnreachableException::class)
    fun addItem(item: Inbox.InboxItemInfo): Common.UniqueId {
        return Common.UniqueId.parseFrom(mMateriaConnection.sendMessage(
                item.toByteString(),
                "InboxService",
                "AddItem"
        ))
    }

    @Throws(InvalidProtocolBufferException::class, MateriaUnreachableException::class)
    fun editItem(item: Inbox.InboxItemInfo): Boolean {
        return Common.OperationResultMessage.parseFrom(mMateriaConnection.sendMessage(
                item.toByteString(),
                "InboxService",
                "EditItem"
        )).success
    }
}
