package snakesoft.minion.materia

import com.google.protobuf.InvalidProtocolBufferException
import common.Common
import strategy.Strategy

class StrategyServiceProxy(private val mMateriaConnection: MateriaConnection)
{
    @Throws(InvalidProtocolBufferException::class, MateriaUnreachableException::class)
    fun loadResources(): Strategy.Resources
    {
        return Strategy.Resources.parseFrom(mMateriaConnection.sendMessage(
                Common.EmptyMessage.newBuilder().build().toByteString(),
                "StrategyService",
                "GetResources"))
    }

    @Throws(InvalidProtocolBufferException::class, MateriaUnreachableException::class)
    fun modifyResource(item: Strategy.Resource): Boolean
    {
        return Common.OperationResultMessage.parseFrom(mMateriaConnection.sendMessage(
                item.toByteString(),
                "StrategyService",
                "ModifyResource"
        )).success
    }
}