package snakesoft.minion.Materia;

import com.google.protobuf.InvalidProtocolBufferException;

import org.zeromq.ZMQ;

public class MateriaConnection
{
    private String mIp;

    public MateriaConnection(String ip)
    {
        mIp = ip;
    }

    public String getIp()
    {
        return mIp;
    }

    public void setNewIp(String ip)
    {
        mIp = ip;
        mConnected = false;
    }

    void connect()
    {
        mContext = ZMQ.context(1);
        mSocket = mContext.socket(ZMQ.REQ);
        mSocket.connect("tcp://" + mIp + ":5757");
        mSocket.setReceiveTimeOut(30000);
        mConnected = true;
    }

    com.google.protobuf.ByteString sendMessage(com.google.protobuf.ByteString payload, String serviceName, String operationName) throws InvalidProtocolBufferException, MateriaUnreachableException
    {
        if(!mConnected)
        {
            connect();
        }
        common.Common.MateriaMessage toSend = common.Common.MateriaMessage.newBuilder()
                .setFrom("minion")
                .setTo(serviceName)
                .setOperationName(operationName)
                .setPayload(payload)
                .build();

        mSocket.send(toSend.toByteArray());
        byte[] responce = mSocket.recv();

        if(responce == null)
        {
            throw new MateriaUnreachableException();
        }

        common.Common.MateriaMessage result = common.Common.MateriaMessage.parseFrom(responce);
        return result.getPayload();
    }

    private ZMQ.Context mContext;
    private ZMQ.Socket mSocket;
    private boolean mConnected = false;
}
