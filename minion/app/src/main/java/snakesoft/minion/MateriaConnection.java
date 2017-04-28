package snakesoft.minion;

import android.util.Log;

import com.google.protobuf.InvalidProtocolBufferException;
import com.google.protobuf.MessageLite;

import org.zeromq.ZMQ;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;
import java.net.UnknownHostException;

import common.Common;

public class MateriaConnection
{
    void connect()
    {
        mContext = ZMQ.context(1);
        mSocket = mContext.socket(ZMQ.REQ);
        mSocket.connect("tcp://192.168.10.8:5910");
        mConnected = true;
    }

    com.google.protobuf.ByteString sendMessage(com.google.protobuf.ByteString payload, String serviceName, String operationName) throws InvalidProtocolBufferException
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

        common.Common.MateriaMessage result = common.Common.MateriaMessage.parseFrom(responce);
        return result.getPayload();
    }

    private ZMQ.Context mContext;
    private ZMQ.Socket mSocket;
    private boolean mConnected = false;
}
