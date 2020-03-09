package snakesoft.minion.materia

import com.google.protobuf.InvalidProtocolBufferException

import common.Common
import strategy.Strategy

class StrategyServiceProxy(private val mMateriaConnection: MateriaConnection)
{
    @Throws(InvalidProtocolBufferException::class, MateriaUnreachableException::class)
    fun loadGraph(id: Common.UniqueId): Strategy.GraphDefinition
    {
        return Strategy.GraphDefinition.parseFrom(mMateriaConnection.sendMessage(
                id.toByteString(),
                "StrategyService",
                "GetGraph"))
    }
}
