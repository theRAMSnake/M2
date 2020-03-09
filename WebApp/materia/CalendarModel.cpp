#include "CalendarModel.hpp"

CalendarModel::CalendarModel(ZmqPbChannel& channel)
: mService(channel)
{

}

std::vector<CalendarModel::Item> CalendarModel::query(const std::time_t from, const std::time_t to)
{
   common::TimeRange t;
   t.set_timestampfrom(from);
   t.set_timestampto(to);

   calendar::CalendarItems items;

   mService.getService().Query(nullptr, &t, &items, nullptr);

   std::vector<CalendarModel::Item> result;

   for(auto x : items.items())
   {
      result.push_back({x.id().guid(), x.text(), x.timestamp(), x.reccurencytype()});
   }

   return result;
}

std::vector<CalendarModel::Item> CalendarModel::next(const std::time_t from, const int limit)
{
   calendar::NextQueryParameters t;
   t.set_timestampfrom(from);
   t.set_limit(limit);

   calendar::CalendarItems items;

   mService.getService().Next(nullptr, &t, &items, nullptr);

   std::vector<CalendarModel::Item> result;

   for(auto x : items.items())
   {
      result.push_back({x.id().guid(), x.text(), x.timestamp()});
   }

   return result;
}

materia::Id CalendarModel::add(const Item& item)
{
   calendar::CalendarItem itemToAdd;
   itemToAdd.set_timestamp(item.timestamp);
   itemToAdd.set_text(item.text);
   itemToAdd.set_reccurencytype(item.reccurencyType);

   common::UniqueId id;
   mService.getService().AddItem(nullptr, &itemToAdd, &id, nullptr);

   return id.guid();
}

void CalendarModel::replace(const Item& item)
{
   calendar::CalendarItem itemToEdit;
   itemToEdit.mutable_id()->set_guid(item.id.getGuid());
   itemToEdit.set_timestamp(item.timestamp);
   itemToEdit.set_text(item.text);
   itemToEdit.set_reccurencytype(item.reccurencyType);

   common::OperationResultMessage dummy;
   mService.getService().EditItem(nullptr, &itemToEdit, &dummy, nullptr);
}

void CalendarModel::erase(const materia::Id& id)
{
   common::UniqueId idMsg;
   idMsg.set_guid(id.getGuid());

   common::OperationResultMessage dummy;
   mService.getService().DeleteItem(nullptr, &idMsg, &dummy, nullptr);
}