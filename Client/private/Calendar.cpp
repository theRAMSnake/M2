#include "Calendar.hpp"
#include "ProtoConverter.hpp"

namespace materia
{

Calendar::Calendar(materia::ZmqPbChannel& channel)
: mProxy(channel)
{

}

bool Calendar::deleteItem(const Id& id)
{
   common::UniqueId protoid = toProto(id);  
   common::OperationResultMessage result;

   mProxy.getService().DeleteItem(nullptr, &protoid, &result, nullptr);
   return result.success();
}

calendar::CalendarItem toProto(const CalendarItem& in)
{
   calendar::CalendarItem out;

   out.mutable_id()->CopyFrom(toProto(in.id));
   out.set_text(in.text);
   out.set_timestamp(in.timestamp);

   return out;
}

CalendarItem fromProto(const calendar::CalendarItem& in)
{
   return { fromProto(in.id()), in.text(), in.timestamp() };
}

bool Calendar::replaceItem(const CalendarItem& item)
{
   calendar::CalendarItem protoItem = toProto(item);
   common::OperationResultMessage result;
   
   mProxy.getService().EditItem(nullptr, &protoItem, &result, nullptr);
   return result.success();
}

Id Calendar::insertItem(const CalendarItem& item)
{
   calendar::CalendarItem protoItem = toProto(item);

   common::UniqueId id;
   mProxy.getService().AddItem(nullptr, &protoItem, &id, nullptr);

   return fromProto(id);
}

std::vector<CalendarItem> Calendar::next(const std::time_t from, const int limit)
{
   calendar::NextQueryParameters query;

   query.set_timestampfrom(from);
   query.set_limit(limit);

   calendar::CalendarItems items;
   
   mProxy.getService().Next(nullptr, &query, &items, nullptr);

   std::vector<CalendarItem> result(items.items_size());
   std::transform(items.items().begin(), items.items().end(), result.begin(), [] (auto x)-> auto { return fromProto(x); });

   return result;
}

std::vector<CalendarItem> Calendar::query(const std::time_t from, const std::time_t to)
{
   calendar::TimeRange timeRange;
   timeRange.set_timestampfrom(from);
   timeRange.set_timestampto(to);

   calendar::CalendarItems items;

   mProxy.getService().Query(nullptr, &timeRange, &items, nullptr);

   std::vector<CalendarItem> result(items.items_size());
   std::transform(items.items().begin(), items.items().end(), result.begin(), [] (auto x)-> auto { return fromProto(x); });

   return result;
}

void Calendar::clear()
{
   common::EmptyMessage r;
   common::OperationResultMessage responce;

   mProxy.getService().Clear(nullptr, &r, &responce, nullptr);
}

}