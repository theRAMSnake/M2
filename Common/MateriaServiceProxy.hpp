#pragma once

#include <Common/ZmqPbChannel.hpp>
#include <Common/PortLayout.hpp>

template<class TService>
class MateriaServiceProxy
{
public:
   MateriaServiceProxy(const std::string& clientName)
   : mContext(1)
   , mSocket(mContext, ZMQ_REQ)
   , mChannel(mSocket, clientName)
   , mService(&mChannel)
   {
      mSocket.connect("tcp://localhost:" + gCentralPort);
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