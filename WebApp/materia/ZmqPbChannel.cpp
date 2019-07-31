#include "ZmqPbChannel.hpp"
#include <google/protobuf/message.h>
#include <messages/common.pb.h>
#include <iostream>
   
ZmqPbChannel::ZmqPbChannel(zmq::socket_t& zmqSocket, const std::string& owner, const std::string& password)
: mZmqSocket(zmqSocket)
, mOwner(owner)
, mCodec(password)
{
}
   
void ZmqPbChannel::CallMethod(const MethodDescriptor * method, RpcController * controller, const Message * request, Message * response, Closure * done)
{
   common::MateriaMessage envelope;
   envelope.set_from(mOwner);
   envelope.set_to(method->service()->name());
   envelope.set_operationname(method->name());
   
   envelope.set_payload(request->SerializeAsString());
   std::string toCode;
   envelope.SerializeToString(&toCode);
   std::string encrypted = mCodec.encrypt(toCode);
   
   zmq::message_t req (encrypted.data(), encrypted.size());
   mZmqSocket.send (req);

   //-----------------------------------------------------------

   zmq::message_t resp;
   mZmqSocket.recv (&resp);

   std::string received(static_cast<const char *>(resp.data()), resp.size());
   std::string decoded;
   
   common::MateriaMessage expectedMessage;

   try
   {
      decoded = mCodec.decrypt(received);
      if(expectedMessage.ParseFromString(decoded) && !expectedMessage.payload().empty())
      {
         response->ParseFromString(expectedMessage.payload());
         return;
      }
   }
   catch(...)
   {
      
   }
   
   if(mErrorCallback != nullptr)
   {
      mErrorCallback(expectedMessage.error());
   }
}

void ZmqPbChannel::setErrorCallback(TErrorCallback errorCallback)
{
   mErrorCallback = errorCallback;
}
