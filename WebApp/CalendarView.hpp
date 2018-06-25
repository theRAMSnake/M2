#pragma once

class MateriaCalendar;
class CalendarItemList;
class CalendarView : public Wt::WContainerWidget
{
public:
    CalendarView(materia::ICalendar& calendar);

private:
   void initiateItemAdd(const Wt::WDate date);
   void initiateItemDelete(const materia::CalendarItem item);
   void initiateItemEdit(const materia::CalendarItem item);

   void onCalendarClicked();

   void updateNextCalendar();

   materia::ICalendar& mCalendar;
   MateriaCalendar* mCalendarWidget;
   CalendarItemList* mDateCalendar;
   CalendarItemList* mNextCalendar;

   int mNextCalendarHighestItemTimestamp = 0;
};