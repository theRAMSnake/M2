#pragma once
#include <memory>
#include <google/protobuf/message.h>

namespace materia
{
   
class MessageFactory
{
public:
   static void init();
   static google::protobuf::Message* queryMessageFromUser(const std::string& messageName);
};
   
}
