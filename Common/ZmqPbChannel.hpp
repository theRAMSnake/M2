#pragma once
#include <google/protobuf/service.h>
#include <zmq.hpp>

using namespace google::protobuf;
namespace materia
{
   
class ZmqPbChannel : public google::protobuf::RpcChannel
{
public:
   ZmqPbChannel(zmq::socket_t& zmqSocket);
   
   virtual void CallMethod(const MethodDescriptor * method, RpcController * controller, const Message * request, Message * response, Closure * done);
   
private:
   zmq::socket_t& mZmqSocket;
};

}
