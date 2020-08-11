package snakesoft.minion.materia

import com.google.protobuf.InvalidProtocolBufferException
import common.Common

@ExperimentalUnsignedTypes
class RewardServiceProxy(private val mMateriaConnection: MateriaConnection)
{
    @Throws(InvalidProtocolBufferException::class, MateriaUnreachableException::class)
    fun addPoints(pts: UInt)
    {
        val jsonData = "{\"operation\": \"reward\", \"points\": $pts}"
        mMateriaConnection.sendMessage(jsonData)
    }
}
