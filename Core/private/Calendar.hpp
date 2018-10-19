#pragma once

#include <boost/date_time/posix_time/posix_time.hpp>
#include <multiset>
#include "../ICalendar.hpp"
#include "Database.hpp"

namespace materia
{

class Calendar : public ICalendar
{
public:
   Calendar(Database& db);

   void deleteItem(const Id& id) override;
   void replaceItem(const CalendarItem& item) override;
   Id insertItem(const CalendarItem& item) override;

   std::vector<CalendarItem> next(const std::time_t from, const int limit) override;
   std::vector<CalendarItem> query(const std::time_t from, const std::time_t to) override;

private:
   std::multiset<CalendarItem, CompareByTime> mItems;
   Database mDb;
};

}