#pragma once
#include <Wt/WContainerWidget.h>
#include "materia/StrategyModel.hpp"

class IGoalViewCtrl;
class StrategyView : public Wt::WContainerWidget
{
public:
   StrategyView(StrategyModel& strategy);

private:
   void onAddGoalClick();
   void onAddTaskClick();
   void onBacklogClick();
   void layGoals();
   
   void putGoal(const StrategyModel::Goal& goal);

   StrategyModel& mModel;
   std::vector<IGoalViewCtrl*> mGoalCtrls;
};