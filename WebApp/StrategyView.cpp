#include "StrategyView.hpp"

StrategyView::StrategyView(materia::IStartegy& strategy, materia::ICalendar& calendar)
: mStrategy(strategy)
{
   auto calendarGroup = new Wt::WGroupBox;
   calendarGroup->addStyleClass("col-md-2");

   calendarGroup->addWidget(std::unique_ptr<Wt::WWidget>(new CalendarView(calendar)));
   addWidget(std::unique_ptr<Wt::WGroupBox>(calendarGroup));

   auto mainGroup = new Wt::WGroupBox;
   mainGroup->addStyleClass("col-md-10");

   addWidget(std::unique_ptr<Wt::WGroupBox>(mainGroup));

   auto t = Wt::cpp14::make_unique<Wt::WTemplate>(TemplateBuilder::makeTable<StrategyCell>(3, 2)));
   addWidget(t);
}