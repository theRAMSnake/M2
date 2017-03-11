#pragma once
#include <Common/ZmqPbChannel.hpp>
#include <iostream>

template<class TService>
class TestServiceProvider
{
public:
   TestServiceProvider()
   : mContext(1)
   , mSocket(mContext, ZMQ_REQ)
   , mChannel(mSocket, "test")
   , mService(&mChannel)
   {
      mSocket.connect("tcp://localhost:5000");
   }
   
   typename TService::Stub& getService()
   {
      return mService;
   }
   
private:
   zmq::context_t mContext;
   zmq::socket_t mSocket;
   materia::ZmqPbChannel mChannel;
   typename TService::Stub mService;
};
