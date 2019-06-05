#pragma once
#include <Wt/WContainerWidget.h>
#include "../materia/StrategyModel.hpp"

class IGoalViewCtrl;
class StrategyView : public Wt::WContainerWidget
{
public:
   StrategyView(StrategyModel& strategy);

private:
   void onAddGoalClick();
   void onAddTaskClick();
   void onAddObjectiveClick();
   void onAddResourceClick();
   void onAddWatchItemClick();
   void onBacklogClick();
   void layGoals();
   void fillResources(Wt::WToolBar& toolbar);
   
   void putGoal(const StrategyModel::Goal& goal);

   void refreshGoalCtrl(const materia::Id& id);

   StrategyModel& mModel;
   Wt::WToolBar* mMainToolbar;
   std::vector<IGoalViewCtrl*> mGoalCtrls;
};