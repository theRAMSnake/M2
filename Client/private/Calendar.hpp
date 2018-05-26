#pragma once

#include "MateriaServiceProxy.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include "../ICalendar.hpp"

#include "messages/calendar.pb.h"

namespace materia
{

class Calendar : public ICalendar
{
public:
   Calendar(materia::ZmqPbChannel& channel);

   bool deleteItem(const Id& id) override;
   bool replaceItem(const CalendarItem& item) override;
   Id insertItem(const CalendarItem& item) override;

   std::vector<CalendarItem> next(const std::time_t from, const int limit) override;
   std::vector<CalendarItem> query(const std::time_t from, const std::time_t to) override;

   void clear() override;

private:
   MateriaServiceProxy<calendar::CalendarService> mProxy;
};

}