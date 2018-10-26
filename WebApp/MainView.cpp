#include "MainView.hpp"
#include <Wt/WGroupBox.h>
#include "CalendarView.hpp"
#include "ActionsView.hpp"

MainView::MainView(CalendarModel& calendar, StrategyModel& strategy, InboxModel& inbox)
{
   auto actionsGroup = new Wt::WGroupBox;
   actionsGroup->addStyleClass("col-md-10");
   auto calendarGroup = new Wt::WGroupBox;
   calendarGroup->addStyleClass("col-md-2");
   
   calendarGroup->addWidget(std::unique_ptr<Wt::WWidget>(new CalendarView(calendar)));
   actionsGroup->addWidget(std::unique_ptr<Wt::WWidget>(new ActionsView(strategy)));
   addWidget(std::unique_ptr<Wt::WGroupBox>(actionsGroup));
   addWidget(std::unique_ptr<Wt::WGroupBox>(calendarGroup));
}