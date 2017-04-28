#include "MessageFactory.hpp"
#include <map>
#include <iostream>
#include <messages/common.pb.h>
#include <messages/inbox.pb.h>

namespace materia
{
   
std::map<std::string, google::protobuf::Message*(*)()> messageBuilders;

#define REGISTER_BUILDER(msgName) messageBuilders.insert(std::make_pair(#msgName, msgName##Builder));

google::protobuf::Message* EmptyMessageBuilder()
{
   return new common::EmptyMessage();
}

google::protobuf::Message* UniqueIdBuilder()
{
   std::cout << "Id: " << std::endl;
   std::string id;
   std::cin >> id;
   auto idMsg = new common::UniqueId();
   idMsg->set_guid(id);

   return idMsg;
}

google::protobuf::Message* InboxItemInfoBuilder()
{
   std::cout << "Id: " << std::endl;
   std::string id;
   std::cin >> id;

   std::cout << "Text: " << std::endl;
   std::string text;
   std::cin >> text;

   auto msg = new inbox::InboxItemInfo();
   msg->mutable_id()->set_guid(id);
   msg->set_text(text);

   return msg;
}
   
void MessageFactory::init()
{
   REGISTER_BUILDER(EmptyMessage);
   REGISTER_BUILDER(InboxItemInfo);
   REGISTER_BUILDER(UniqueId);
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
