#pragma once

#include <Common/Id.hpp>
#include <ctime>

namespace materia
{

enum ReccurencyType
{
   None,
   Weekly,
   Monthly,
   Quarterly,
   Yearly
};

enum class CalendarEntityType
{
   Event,
   Task
};

struct CalendarItem
{
   Id id;
   std::string text;
   std::time_t timestamp;
   ReccurencyType reccurencyType = ReccurencyType::None;
   CalendarEntityType entityType = CalendarEntityType::Event;
};

class ICalendar
{
public:
   virtual void deleteItem(const Id& id) = 0;
   virtual void completeItem(const Id& id) = 0;
   virtual void replaceItem(const CalendarItem& item) = 0;
   virtual Id insertItem(const CalendarItem& item) = 0;

   virtual std::vector<CalendarItem> next(const std::time_t from, const int limit) = 0;
   virtual std::vector<CalendarItem> query(const std::time_t from, const std::time_t to) = 0;

   virtual ~ICalendar(){}
};

}