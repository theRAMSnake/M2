#include "Calendar.hpp"

namespace materia
{

template<>
CalendarItem fromJson(const std::string& json)
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

template<>
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
: mDb(db)
{
    loadItems();
}

void Calendar::deleteItem(const Id& id)
{
   auto pos = find_by_id(mItems, id);
   if(pos != mItems.end())
   {
       mItems.erase(pos);
       mDb.erase(id);
   }
}

void Calendar::replaceItem(const CalendarItem& item)
{
   auto pos = find_by_id(mItems, item.id);
   if(pos != mItems.end())
   {
       *pos = item;

       mDb.store(item.id, toJson(item));
   }
}

Id Calendar::insertItem(const CalendarItem& item)
{
   auto pos = find_by_id(mItems, item.id);
   if(pos == mItems.end())
   {
       auto newItem = item;
       newItem.id = Id::generate();

       mDb.store(newItem.id, toJson(item));

       return newItem.id;
   }

   return Id::Invalid;
}

std::vector<CalendarItem> Calendar::next(const std::time_t from, const int limit)
{
    std::vector<CalendarItem> result;

    CalendarItem fromItem {Id::Invalid, "", from};

    auto it = std::lower_bound(mItems.begin(), mItems.end(), from);
    auto end = it;
    std::advance(end, limit);

    if(end > mItems.end())
    {
        end = mItems.end();
    }

    std::copy(it, end, std::inserter(result));

    return result;
}

std::vector<CalendarItem> Calendar::query(const std::time_t from, const std::time_t to)
{
    std::vector<CalendarItem> result;

    CalendarItem fromItem {Id::Invalid, "", from};
    CalendarItem toItem {Id::Invalid, "", to};

    auto it = std::lower_bound(mItems.begin(), mItems.end(), from);
    auto end = std::lower_bound(mItems.begin(), mItems.end(), to);
    
    std::copy(it, end, std::inserter(result));

    return result;
}

}