#include "ZmqPbChannel.hpp"
#include <google/protobuf/message.h>
#include <messages/common.pb.h>
#include <iostream>

namespace materia
{
   
ZmqPbChannel::ZmqPbChannel(zmq::socket_t& zmqSocket, const std::string& owner)
: mZmqSocket(zmqSocket)
, mOwner(owner)
{
}
   
void ZmqPbChannel::CallMethod(const MethodDescriptor * method, RpcController * controller, const Message * request, Message * response, Closure * done)
{
   common::MateriaMessage envelope;
   envelope.set_from(mOwner);
   envelope.set_to(method->service()->name());
   envelope.set_operationname(method->name());
   
   envelope.set_payload(request->SerializeAsString());
   
   zmq::message_t req (envelope.ByteSizeLong());
   envelope.SerializeToArray(req.data (), req.size());
   
   mZmqSocket.send (req);
   
   zmq::message_t resp;
   mZmqSocket.recv (&resp);
   
   common::MateriaMessage expectedMessage;
   expectedMessage.ParseFromArray(resp.data(), resp.size());
   
   if(expectedMessage.payload().empty())
   {
      if(mErrorCallback != nullptr)
      {
         mErrorCallback(expectedMessage.error());
      }
   }
   else
   {
      response->ParseFromString(expectedMessage.payload());
   }
}

void ZmqPbChannel::setErrorCallback(TErrorCallback errorCallback)
{
   mErrorCallback = errorCallback;
}

}
