#include "MessageFactory.hpp"
#include <map>
#include <messages/common.pb.h>

namespace materia
{
   
std::map<std::string, google::protobuf::Message*(*)()> messageBuilders;

#define REGISTER_BUILDER(msgName) messageBuilders.insert(std::make_pair(#msgName, msgName##Builder));

google::protobuf::Message* EmptyMessageBuilder()
{
   return new common::EmptyMessage();
}
   
void MessageFactory::init()
{
   REGISTER_BUILDER(EmptyMessage);
}

google::protobuf::Message* MessageFactory::queryMessageFromUser(const std::string& messageName)
{
   google::protobuf::Message* result;
   
   auto pos = messageBuilders.find(messageName);
   if(pos != messageBuilders.end())
   {
      result = pos->second();
   }
      
   return result;
}

}
