#include "CalendarModel.hpp"

CalendarModel::CalendarModel(ZmqPbChannel& channel)
: mService(channel)
{

}


calendar::CalendarItem toProto(const CalendarModel::Item& x)
{
   calendar::CalendarItem result;

   result.mutable_id()->set_guid(x.id.getGuid());
   result.set_timestamp(x.timestamp);
   result.set_text(x.text);
   result.set_reccurencytype(static_cast<calendar::ReccurencyType>(x.reccurencyType));
   result.set_entitytype(static_cast<calendar::EntityType>(x.entityType));

   return result;
}

CalendarModel::Item fromProto(const calendar::CalendarItem& x)
{
   return {x.id().guid(), x.text(), x.timestamp(), x.reccurencytype(), x.entitytype()};
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
      result.push_back(fromProto(x));
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
      result.push_back(fromProto(x));
   }

   return result;
}

materia::Id CalendarModel::add(const Item& item)
{
   calendar::CalendarItem itemToAdd = toProto(item);

   common::UniqueId id;
   mService.getService().AddItem(nullptr, &itemToAdd, &id, nullptr);

   return id.guid();
}

void CalendarModel::replace(const Item& item)
{
   calendar::CalendarItem itemToEdit = toProto(item);

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

void CalendarModel::complete(const materia::Id& id)
{
   common::UniqueId idMsg;
   idMsg.set_guid(id.getGuid());

   common::OperationResultMessage dummy;
   mService.getService().CompleteItem(nullptr, &idMsg, &dummy, nullptr);
}