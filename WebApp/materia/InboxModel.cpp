#include "InboxModel.hpp"

InboxModel::InboxModel(ZmqPbChannel& channel)
: mService(channel)
{

}

std::vector<InboxModel::Item> InboxModel::get()
{
   common::EmptyMessage e;

   inbox::InboxItems items;

   mService.getService().GetInbox(nullptr, &e, &items, nullptr);

   std::vector<InboxModel::Item> result;

   for(auto x : items.items())
   {
      result.push_back({x.id().guid(), x.text()});
   }

   return result;
}

materia::Id InboxModel::add(const Item& item)
{
   inbox::InboxItemInfo itemToAdd;
   itemToAdd.set_text(item.text);

   common::UniqueId id;
   mService.getService().AddItem(nullptr, &itemToAdd, &id, nullptr);

   return id.guid();
}

void InboxModel::replace(const Item& item)
{
   inbox::InboxItemInfo itemToEdit;
   itemToEdit.mutable_id()->set_guid(item.id.getGuid());
   itemToEdit.set_text(item.text);

   common::OperationResultMessage dummy;
   mService.getService().EditItem(nullptr, &itemToEdit, &dummy, nullptr);
}

void InboxModel::erase(const materia::Id& id)
{
   common::UniqueId idMsg;
   idMsg.set_guid(id.getGuid());

   common::OperationResultMessage dummy;
   mService.getService().DeleteItem(nullptr, &idMsg, &dummy, nullptr);
}

void InboxModel::clear()
{
   for(auto& i : get())
   {
      erase(i.id);
   }
}