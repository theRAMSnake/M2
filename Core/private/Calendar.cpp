#include "Calendar.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace materia
{

CalendarItem createCalendarItemFromJson(const std::string& json)
{
   CalendarItem result;

   boost::property_tree::ptree pt;
   std::istringstream is (json);
   read_json (is, pt);
   
   result.id = pt.get<std::string> ("id");
   result.text = pt.get<std::string> ("text");
   result.timestamp = pt.get<std::time_t> ("timestamp");

   return result;
}

std::string toJson(const CalendarItem& from)
{
   boost::property_tree::ptree pt;

   pt.put ("id", from.id.getGuid());
   pt.put ("text", from.text);
   pt.put ("timestamp", from.timestamp);

   std::ostringstream buf; 
   write_json (buf, pt, false);
   return buf.str();
}

Calendar::Calendar(Database& db)
: mStorage(db.getTable("calendar"))
{
    mStorage->foreach([&](std::string id, std::string json) 
    {
        mItems.insert(createCalendarItemFromJson(json));
    });
}

void Calendar::deleteItem(const Id& id)
{
   auto pos = find_by_id(mItems, id);
   if(pos != mItems.end())
   {
       mItems.erase(pos);
       mStorage->erase(id);
   }
}

void Calendar::replaceItem(const CalendarItem& item)
{
   auto pos = find_by_id(mItems, item.id);
   if(pos != mItems.end())
   {
       mItems.erase(pos);
       mItems.insert(item);

       mStorage->store(item.id, toJson(item));
   }
}

Id Calendar::insertItem(const CalendarItem& item)
{
   auto pos = find_by_id(mItems, item.id);
   if(pos == mItems.end())
   {
       auto newItem = item;
       newItem.id = Id::generate();

       mStorage->store(newItem.id, toJson(item));

       return newItem.id;
   }

   return Id::Invalid;
}

std::vector<CalendarItem> Calendar::next(const std::time_t from, const int limit)
{
    std::vector<CalendarItem> result;

    CalendarItem fromItem {Id::Invalid, "", from};

    CompareByTime c;
    auto it = std::lower_bound(mItems.begin(), mItems.end(), fromItem, c);
    auto end = it;

    for(int i = 0; i < limit && end != mItems.end(); ++end, ++i);

    std::copy(it, end, std::inserter(result, result.begin()));

    return result;
}

std::vector<CalendarItem> Calendar::query(const std::time_t from, const std::time_t to)
{
    std::vector<CalendarItem> result;

    CalendarItem fromItem {Id::Invalid, "", from};
    CalendarItem toItem {Id::Invalid, "", to};

    CompareByTime c;
    auto it = std::lower_bound(mItems.begin(), mItems.end(), fromItem, c);
    auto end = std::lower_bound(mItems.begin(), mItems.end(), toItem, c);
    
    std::copy(it, end, std::inserter(result, result.begin()));

    return result;
}

}