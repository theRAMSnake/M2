#include "ZmqPbChannel.hpp"
#include <google/protobuf/message.h>
#include <messages/common.pb.h>
#include <iostream>

namespace materia
{
   
ZmqPbChannel::ZmqPbChannel(zmq::socket_t& zmqSocket)
: mZmqSocket(zmqSocket)
{
}
   
void ZmqPbChannel::CallMethod(const MethodDescriptor * method, RpcController * controller, const Message * request, Message * response, Closure * done)
{
   zmq::message_t req (request->ByteSizeLong());
   request->SerializeToArray(req.data (), req.size());
   
   mZmqSocket.send (req);
   
   zmq::message_t resp;
   mZmqSocket.recv (&resp);
   
   common::MateriaMessage expectedMessage;
   expectedMessage.ParseFromArray(resp.data(), resp.size());
   
   if(expectedMessage.payload().empty())
   {
      //LOG_WARNING_HERE
   }
   else
   {
      response->ParseFromString(expectedMessage.payload());
   }
}

}
