package snakesoft.minion.materia

import org.zeromq.ZMQ

//Encapsulates ZMQ communication with materia server
class MateriaConnection(ip: String)
{
    private var mSocket: ZMQ.Socket
    private var mContext: ZMQ.Context = ZMQ.context(1)

    init
    {
        mSocket = mContext.socket(ZMQ.REQ)
        mSocket.connect("tcp://$ip:5757")
        mSocket.receiveTimeOut = 30000
    }

    @Throws(MateriaUnreachableException::class)
    internal fun sendMessage(payload: com.google.protobuf.ByteString, serviceName: String, operationName: String): com.google.protobuf.ByteString
    {
        val toSend = common.Common.MateriaMessage.newBuilder()
                .setFrom("minion")
                .setTo(serviceName)
                .setOperationName(operationName)
                .setPayload(payload)
                .build()

        mSocket.send(toSend.toByteArray())
        val response = mSocket.recv() ?: throw MateriaUnreachableException()

        val result = common.Common.MateriaMessage.parseFrom(response)
        return result.payload
    }
}
