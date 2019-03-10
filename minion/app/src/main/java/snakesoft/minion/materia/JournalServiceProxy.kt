package snakesoft.minion.materia

import com.google.protobuf.InvalidProtocolBufferException

import journal.Journal
import common.Common

class JournalServiceProxy(private val mMateriaConnection: MateriaConnection) {

    @Throws(InvalidProtocolBufferException::class, MateriaUnreachableException::class)
    fun loadIndex(): Journal.Index
    {
        return Journal.Index.parseFrom(mMateriaConnection.sendMessage(
                Common.EmptyMessage.newBuilder().build().toByteString(),
                "JournalService",
                "GetIndex"))
    }

    @Throws(InvalidProtocolBufferException::class, MateriaUnreachableException::class)
    fun loadPage(id: Common.UniqueId): Journal.Page
    {
        return Journal.Page.parseFrom(mMateriaConnection.sendMessage(
                id.toByteString(),
                "JournalService",
                "GetPage"))
    }
}
