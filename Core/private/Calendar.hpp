#pragma once

#include <boost/date_time/posix_time/posix_time.hpp>
#include <set>
#include "../ICalendar.hpp"
#include "../IReward.hpp"
#include "Database.hpp"

namespace materia
{

struct CompareByTime 
{
    bool operator() (const CalendarItem& lhs, const CalendarItem& rhs) const 
    {
        return lhs.timestamp < rhs.timestamp;    
    }
};

class Calendar : public ICalendar
{
public:
   Calendar(Database& db, IReward& reward);

   void deleteItem(const Id& id) override;
   void completeItem(const Id& id) override;
   void replaceItem(const CalendarItem& item) override;
   Id insertItem(const CalendarItem& item) override;

   std::vector<CalendarItem> next(const std::time_t from, const int limit) override;
   std::vector<CalendarItem> query(const std::time_t from, const std::time_t to) override;

private:
   std::multiset<CalendarItem, CompareByTime> mItems;
   std::unique_ptr<DatabaseTable> mStorage;

   IReward& mReward;
};

}