#include "MessageFactory.hpp"
#include <map>
#include <iostream>
#include <messages/common.pb.h>
#include <messages/inbox.pb.h>
#include <messages/actions.pb.h>

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
   std::getline(std::cin, text);
   std::getline(std::cin, text);

   auto msg = new inbox::InboxItemInfo();
   msg->mutable_id()->set_guid(id);
   msg->set_text(text);

   return msg;
}

google::protobuf::Message* ActionInfoBuilder()
{
   std::cout << "Id: " << std::endl;
   std::string id;
   std::cin >> id;

   std::cout << "ParentId: (-1 for none)" << std::endl;
   std::string parentId;
   std::cin >> parentId;

   std::cout << "Title: " << std::endl;
   std::string title;
   std::cin >> title;

   std::cout << "Description: " << std::endl;
   std::string description;
   std::cin >> description;

   std::cout << "Type (0 - task, 1 - group): " << std::endl;
   int atype;
   std::cin >> atype;

   auto msg = new actions::ActionInfo();
   msg->mutable_id()->set_guid(id);
   if(parentId != "-1")
   {
      msg->mutable_parentid()->set_guid(parentId);
   }
   
   msg->set_title(title);
   msg->set_description(description);
   msg->set_type(static_cast<actions::ActionType>(atype));

   return msg;
}
   
void MessageFactory::init()
{
   REGISTER_BUILDER(EmptyMessage);
   REGISTER_BUILDER(InboxItemInfo);
   REGISTER_BUILDER(UniqueId);
   REGISTER_BUILDER(ActionInfo);
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
