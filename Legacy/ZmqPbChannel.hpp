#pragma once
#include <google/protobuf/service.h>
#include <zmq.hpp>
#include <functional>

using namespace google::protobuf;
namespace materia
{
   
class ZmqPbChannel : public google::protobuf::RpcChannel
{
public:
   typedef std::function<void(const std::string&)> TErrorCallback;
   ZmqPbChannel(zmq::socket_t& zmqSocket, const std::string& owner);
   
   void setErrorCallback(TErrorCallback errorCallback);
   
   virtual void CallMethod(const MethodDescriptor * method, RpcController * controller, const Message * request, Message * response, Closure * done);
   
private:
   zmq::socket_t& mZmqSocket;
   std::string mOwner;
   TErrorCallback mErrorCallback;
};

}
