package snakesoft.minion.materia

import com.google.protobuf.InvalidProtocolBufferException

import common.Common
import finance.Finance

class FinanceServiceProxy(private val mMateriaConnection: MateriaConnection)
{
    @Throws(InvalidProtocolBufferException::class, MateriaUnreachableException::class)
    fun loadCategories(): Finance.CategoryItems
    {
        return Finance.CategoryItems.parseFrom(mMateriaConnection.sendMessage(
                Common.EmptyMessage.newBuilder().build().toByteString(),
                "FinanceService",
                "GetCategories"))
    }

    @Throws(InvalidProtocolBufferException::class, MateriaUnreachableException::class)
    fun addEvent(item: Finance.EventInfo): Common.UniqueId
    {
        return Common.UniqueId.parseFrom(mMateriaConnection.sendMessage(
                item.toByteString(),
                "FinanceService",
                "AddEvent"
        ))
    }
}
