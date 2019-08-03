package snakesoft.minion.materia

import com.google.protobuf.InvalidProtocolBufferException

import common.Common
import strategy.Strategy

class StrategyServiceProxy(private val mMateriaConnection: MateriaConnection)
{
    @Throws(InvalidProtocolBufferException::class, MateriaUnreachableException::class)
    fun loadFocusItems(): Strategy.FocusItems
    {
        return Strategy.FocusItems.parseFrom(mMateriaConnection.sendMessage(
                Common.EmptyMessage.newBuilder().build().toByteString(),
                "StrategyService",
                "GetFocusItems"))
    }

    @Throws(InvalidProtocolBufferException::class, MateriaUnreachableException::class)
    fun loadGraph(id: Common.UniqueId): Strategy.GraphDefinition
    {
        return Strategy.GraphDefinition.parseFrom(mMateriaConnection.sendMessage(
                id.toByteString(),
                "StrategyService",
                "GetGraph"))
    }

    @Throws(InvalidProtocolBufferException::class, MateriaUnreachableException::class)
    fun completeFocusItem(item: Strategy.FocusItemInfo)
    {
        mMateriaConnection.sendMessage(
                item.toByteString(),
                "StrategyService",
                "CompleteFocusItem"
        )
    }
}
