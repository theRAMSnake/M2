#include "Calendar.hpp"

namespace materia
{

Calendar::Calendar(materia::ZmqPbChannel& channel)
: mProxy(channel)
{

}

bool Calendar::deleteItem(const Id& id)
{
   common::UniqueId protoid = id.toProtoId();  
   common::OperationResultMessage result;

   mProxy.getService().DeleteItem(nullptr, &protoid, &result, nullptr);
   return result.success();
}

calendar::CalendarItem convert(const CalendarItem& in)
{
   calendar::CalendarItem out;

   out.mutable_id()->CopyFrom(in.id.toProtoId());
   out.set_text(in.text);
   out.set_timestamp(boost::posix_time::to_time_t(in.timestamp));

   return out;
}

CalendarItem convert(const calendar::CalendarItem& in)
{
   return { Id(in.id()), in.text(), boost::posix_time::from_time_t(in.timestamp()) };
}

bool Calendar::replaceItem(const CalendarItem& item)
{
   calendar::CalendarItem protoItem = convert(item);
   common::OperationResultMessage result;
   
   mProxy.getService().EditItem(nullptr, &protoItem, &result, nullptr);
   return result.success();
}

Id Calendar::insertItem(const CalendarItem& item)
{
   calendar::CalendarItem protoItem = convert(item);

   common::UniqueId id;
   mProxy.getService().AddItem(nullptr, &protoItem, &id, nullptr);

   return Id(id);
}

std::vector<CalendarItem> Calendar::next(const boost::posix_time::ptime from, const int limit)
{
   calendar::NextQueryParameters query;

   query.set_timestampfrom(boost::posix_time:: to_time_t(from));
   query.set_limit(limit);

   calendar::CalendarItems items;
   
   mProxy.getService().Next(nullptr, &query, &items, nullptr);

   std::vector<CalendarItem> result(items.items_size());
   std::transform(items.items().begin(), items.items().end(), result.begin(), [] (auto x)-> auto { return convert(x); });

   return result;
}

std::vector<CalendarItem> Calendar::query(const boost::posix_time::ptime from, const boost::posix_time::ptime to)
{
   calendar::TimeRange timeRange;
   timeRange.set_timestampfrom(boost::posix_time:: to_time_t(from));
   timeRange.set_timestampto(boost::posix_time:: to_time_t(to));

   calendar::CalendarItems items;

   mProxy.getService().Query(nullptr, &timeRange, &items, nullptr);

   std::vector<CalendarItem> result(items.items_size());
   std::transform(items.items().begin(), items.items().end(), result.begin(), [] (auto x)-> auto { return convert(x); });

   return result;
}

}