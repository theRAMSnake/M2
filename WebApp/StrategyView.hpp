#pragma once
#include <Wt/WContainerWidget.h>
#include "materia/StrategyModel.hpp"

class StrategyView : public Wt::WContainerWidget
{
public:
   StrategyView(StrategyModel& strategy);

private:
   void onAddClick();
   void putGoal(const StrategyModel::Goal& goal);

   StrategyModel& mModel;
};