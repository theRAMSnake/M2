#include "Calendar.hpp"
#include "Logger.hpp"
#include "JsonSerializer.hpp"

BIND_JSON3(materia::CalendarItem, id, text, timestamp)

namespace materia
{

Calendar::Calendar(Database& db)
: mStorage(db.getTable("calendar"))
{
    LOG("Start calendar init");

    mStorage->foreach([&](std::string id, std::string json) 
    {
        LOG("Read: " + json);
        mItems.insert(readJson<CalendarItem>(json));
    });

    LOG("End calendar init");
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

       mStorage->store(item.id, writeJson(item));
   }
}

Id Calendar::insertItem(const CalendarItem& item)
{
    auto newItem = item;
    newItem.id = Id::generate();

    mItems.insert(newItem);
    mStorage->store(newItem.id, writeJson(newItem));

    return newItem.id;
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