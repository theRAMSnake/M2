#pragma once
#include <Wt/WContainerWidget.h>
#include <Client/IStrategy.hpp>

class StrategyView : public Wt::WContainerWidget
{
public:
   StrategyView(materia::IStrategy& strategy, materia::ICalendar& calendar);

private:
   materia::IStrategy& mStrategy;
};