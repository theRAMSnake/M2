package snakesoft.minion.materia

import com.google.protobuf.InvalidProtocolBufferException
import common.Common
import freedata.Freedata

class FreeDataServiceProxy(private val mMateriaConnection: MateriaConnection)
{
    @Throws(InvalidProtocolBufferException::class, MateriaUnreachableException::class)
    fun get(): Freedata.FreeDataBlocks
    {
        return Freedata.FreeDataBlocks.parseFrom(mMateriaConnection.sendMessage(
                Common.EmptyMessage.newBuilder().build().toByteString(),
                "FreeDataService",
                "Get"))
    }

    @Throws(InvalidProtocolBufferException::class, MateriaUnreachableException::class)
    fun increment(name: String, delta: Int): Boolean
    {
        val b = Freedata.FreeDataBlock.newBuilder()

        b.name = name
        b.value = delta

        return Common.OperationResultMessage.parseFrom(mMateriaConnection.sendMessage(
                b.build().toByteString(),
                "FreeDataService",
                "Increment"
        )).success
    }
}