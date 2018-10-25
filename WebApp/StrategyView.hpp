#pragma once
#include <Wt/WContainerWidget.h>
#include "materia/StrategyModel.hpp"

class StrategyView : public Wt::WContainerWidget
{
public:
   StrategyView(StrategyModel& strategy);

private:
   StrategyModel& mStrategy;
};