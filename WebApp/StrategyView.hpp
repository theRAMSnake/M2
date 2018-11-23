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
   void onAddResourceClick();
   void onBacklogClick();
   void layGoals();
   void fillResources(Wt::WToolBar& toolbar);
   
   void putGoal(const StrategyModel::Goal& goal);

   StrategyModel& mModel;
   std::vector<IGoalViewCtrl*> mGoalCtrls;
};