#include "Calendar.hpp"
#include "Logger.hpp"
#include "JsonSerializer.hpp"
#include <boost/date_time/date_duration_types.hpp>

SERIALIZE_AS_INTEGER(materia::ReccurencyType)
BIND_JSON4(materia::CalendarItem, id, text, timestamp, reccurencyType)

namespace materia
{

Calendar::Calendar(Database& db)
: mStorage(db.getTable("calendar"))
{
    mStorage->foreach([&](std::string id, std::string json) 
    {
        mItems.insert(readJson<CalendarItem>(json));
    });
}

CalendarItem advance(const CalendarItem& src)
{
    CalendarItem result = src;

    switch (src.reccurencyType)
    {
    case ReccurencyType::Weekly:
        result.timestamp += 604800;
        break;

    case ReccurencyType::Monthly:
        result.timestamp = boost::posix_time::to_time_t(boost::posix_time::from_time_t(result.timestamp) + boost::gregorian::months(1));
        break;

    case ReccurencyType::Quarterly:
        result.timestamp = boost::posix_time::to_time_t(boost::posix_time::from_time_t(result.timestamp) + boost::gregorian::months(3));
        break;

    case ReccurencyType::Yearly:
        result.timestamp = boost::posix_time::to_time_t(boost::posix_time::from_time_t(result.timestamp) + boost::gregorian::months(12));
        break;
    
    default:
        break;
    }

    return result;
}

void Calendar::deleteItem(const Id& id)
{
   auto pos = find_by_id(mItems, id);
   if(pos != mItems.end())
   {
       if(pos->reccurencyType != ReccurencyType::None)
       {
           replaceItem(advance(*pos));
       }
       else
       {
           mItems.erase(pos);
           mStorage->erase(id);
       }
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