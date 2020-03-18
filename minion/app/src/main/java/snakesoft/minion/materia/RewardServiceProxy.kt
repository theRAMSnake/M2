package snakesoft.minion.materia

import com.google.protobuf.InvalidProtocolBufferException
import common.Common

@ExperimentalUnsignedTypes
class RewardServiceProxy(private val mMateriaConnection: MateriaConnection)
{
    @Throws(InvalidProtocolBufferException::class, MateriaUnreachableException::class)
    fun addPoints(pts: UInt)
    {
        val msg = Common.IntMessage.newBuilder().setContent(pts.toInt()).build()

        mMateriaConnection.sendMessage(
            msg.toByteString(),
            "RewardService",
            "AddPoints"
        )
    }
}
