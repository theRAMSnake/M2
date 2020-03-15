#pragma once

#include <Wt/WContainerWidget.h>
#include "materia/CalendarModel.hpp"

class MateriaCalendar;
class CalendarItemList;
class CalendarView : public Wt::WContainerWidget
{
public:
   CalendarView(CalendarModel& calendar);

private:
   void initiateItemAdd(const Wt::WDate date);
   void initiateItemResolve(const CalendarModel::Item item);
   void initiateItemEdit(const CalendarModel::Item item);

   void onCalendarClicked();

   void updateNextCalendar();

   CalendarModel& mCalendar;
   MateriaCalendar* mCalendarWidget;
   CalendarItemList* mDateCalendar;
   CalendarItemList* mNextCalendar;

   int mNextCalendarHighestItemTimestamp = 0;
};