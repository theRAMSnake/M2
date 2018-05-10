#include "Inbox.hpp"
#include "ProtoConverter.hpp"

namespace materia
{

Inbox::Inbox(materia::ZmqPbChannel& channel)
: mProxy(channel)
{

}

std::vector<InboxItem> Inbox::getItems()
{
   common::EmptyMessage emptyMsg;
   inbox::InboxItems responce;
   
   mProxy.getService().GetInbox(nullptr, &emptyMsg, &responce, nullptr);

   std::vector<InboxItem> result;

   for(auto x : responce.items())
   {
      result.push_back({fromProto(x.id()), x.text()});
   }

   return result;
}

bool Inbox::deleteItem(const Id& id)
{
   common::UniqueId request;
   request.CopyFrom(toProto(id));
   
   common::OperationResultMessage opResult;
   mProxy.getService().DeleteItem(nullptr, &request, &opResult, nullptr);
   
   return opResult.success();
}

bool Inbox::replaceItem(const InboxItem& item)
{
   inbox::InboxItemInfo request;
   request.set_text(item.text);
   request.mutable_id()->CopyFrom(toProto(item.id));
   
   common::OperationResultMessage opResult;
   
   mProxy.getService().EditItem(nullptr, &request, &opResult, nullptr);
   
   return opResult.success();
}

Id Inbox::insertItem(const InboxItem& item)
{
   inbox::InboxItemInfo request;
   request.set_text(item.text);
   
   common::UniqueId responce;
   
   mProxy.getService().AddItem(nullptr, &request, &responce, nullptr);

   return fromProto(responce);
}

}