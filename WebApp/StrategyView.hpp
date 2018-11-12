#pragma once
#include <Wt/WContainerWidget.h>
#include "materia/StrategyModel.hpp"

class GoalViewCtrl;
class StrategyView : public Wt::WContainerWidget
{
public:
   StrategyView(StrategyModel& strategy);

private:
   void onAddClick();
   void onBacklogClick();
   void layGoals();
   
   void putGoal(const StrategyModel::Goal& goal);

   StrategyModel& mModel;
   std::vector<GoalViewCtrl*> mGoalCtrls;
};