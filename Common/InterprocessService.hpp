#pragma once
#include "ServiceWrapper.hpp"
#include <zmq.hpp>
#include <fstream>

namespace materia
{
   
template<class TServiceProvider>
class InterprocessService
{
public:
   InterprocessService(TServiceProvider& service)
   : mServiceProvider(service)
   {
   }
   
   void provideAt(const std::string& path, const std::string& name)
   {
      mSocket.bind("tcp://" + path);
      std::ofstream log((name + ".log").c_str());
      
      while(true)
      {
         zmq::message_t request;
         mSocket.recv (&request);
        
         common::MateriaMessage requestMsg;
         requestMsg.ParseFromArray(request.data(), request.size());
         
         common::MateriaMessage responceMsg = mServiceProvider.sendMessage(requestMsg);
        
         log << "Received: " << requestMsg.ShortDebugString() << std::endl;
        
         zmq::message_t responce (responceMsg.ByteSizeLong());
         responceMsg.SerializeToArray(responce.data (), responce.size());
        
         log << "Sent: " << responceMsg.ShortDebugString() << std::endl;
      
         mSocket.send (responce);
      }
   }
   
private:
   zmq::context_t mContext = zmq::context_t(1);
   zmq::socket_t mSocket = zmq::socket_t(mContext, ZMQ_REP);
   materia::ServiceWrapper<TServiceProvider> mServiceProvider;
};
}
