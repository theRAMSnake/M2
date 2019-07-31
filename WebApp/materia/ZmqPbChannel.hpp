#pragma once
#include <google/protobuf/service.h>
#include <zmq.hpp>
#include <functional>
#include "Common/Codec.hpp"

using namespace google::protobuf;
   
class ZmqPbChannel : public google::protobuf::RpcChannel
{
public:
   typedef std::function<void(const std::string&)> TErrorCallback;
   ZmqPbChannel(zmq::socket_t& zmqSocket, const std::string& owner, const std::string& password);
   
   void setErrorCallback(TErrorCallback errorCallback);
   
   virtual void CallMethod(const MethodDescriptor * method, RpcController * controller, const Message * request, Message * response, Closure * done);
   
private:
   zmq::socket_t& mZmqSocket;
   std::string mOwner;
   Codec mCodec;
   TErrorCallback mErrorCallback;
};
