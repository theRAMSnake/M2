#pragma once
#include <Wt/WContainerWidget.h>
#include <Wt/WCalendar.h>
#include "materia/StrategyModel.hpp"

class ActionsView : public Wt::WContainerWidget
{
public:
   ActionsView(StrategyModel& strategy);

private:
   StrategyModel& mStrategy;
};