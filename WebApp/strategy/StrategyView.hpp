#pragma once
#include <Wt/WContainerWidget.h>
#include "../materia/StrategyModel.hpp"

class IGoalViewCtrl;
class StrategyView : public Wt::WContainerWidget
{
public:
   StrategyView(StrategyModel& strategy, ChallengeModel& chModel);

private:
   void onAddGoalClick();
   void onAddWatchItemClick();
   void onBacklogClick();
   void layGoals();
   
   void putGoal(const StrategyModel::Goal& goal);

   void refreshGoalCtrl(const materia::Id& id);

   StrategyModel& mStrategyModel;
   ChallengeModel& mChModel;
   Wt::WToolBar* mMainToolbar;
   std::vector<IGoalViewCtrl*> mGoalCtrls;
};