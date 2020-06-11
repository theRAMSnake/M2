#pragma once

#include <Wt/WContainerWidget.h>

class CalendarModel;
class FinanceModel;
class JournalModel;
class MainView : public Wt::WContainerWidget
{
public:
   MainView(CalendarModel& calendar, FinanceModel& finance, JournalModel& journal);

private:
   
};