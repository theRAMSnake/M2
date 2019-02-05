package snakesoft.minion.materia

import com.google.protobuf.InvalidProtocolBufferException

import common.Common
import inbox.Inbox

class InboxServiceProxy(private val mMateriaConnection: MateriaConnection)
{
    @Throws(InvalidProtocolBufferException::class, MateriaUnreachableException::class)
    fun loadInbox(): Inbox.InboxItems
    {
        return Inbox.InboxItems.parseFrom(mMateriaConnection.sendMessage(
            Common.EmptyMessage.newBuilder().build().toByteString(),
            "InboxService",
            "GetInbox"))
    }

    @Throws(InvalidProtocolBufferException::class, MateriaUnreachableException::class)
    fun deleteItem(id: Common.UniqueId)
    {
        mMateriaConnection.sendMessage(
                id.toByteString(),
                "InboxService",
                "DeleteItem"
        )
    }

    @Throws(InvalidProtocolBufferException::class, MateriaUnreachableException::class)
    fun addItem(item: Inbox.InboxItemInfo): Common.UniqueId
    {
        return Common.UniqueId.parseFrom(mMateriaConnection.sendMessage(
                item.toByteString(),
                "InboxService",
                "AddItem"
        ))
    }

    @Throws(InvalidProtocolBufferException::class, MateriaUnreachableException::class)
    fun editItem(item: Inbox.InboxItemInfo): Boolean
    {
        return Common.OperationResultMessage.parseFrom(mMateriaConnection.sendMessage(
                item.toByteString(),
                "InboxService",
                "EditItem"
        )).success
    }
}
