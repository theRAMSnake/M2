#pragma once

#include "MateriaServiceProxy.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include "Id.hpp"

#include "messages/calendar.pb.h"

#include <boost/optional.hpp>

namespace materia
{

struct CalendarItem
{
   Id id;
   std::string text;
   boost::posix_time::ptime timestamp;
};

class Calendar
{
public:
   Calendar(materia::ZmqPbChannel& channel);

   bool deleteItem(const Id& id);
   bool replaceItem(const CalendarItem& item);
   Id insertItem(const CalendarItem& item);

   std::vector<CalendarItem> next(const boost::posix_time::ptime from, const int limit);
   std::vector<CalendarItem> query(const boost::posix_time::ptime from, const boost::posix_time::ptime to);

private:
   MateriaServiceProxy<calendar::CalendarService> mProxy;
};

}