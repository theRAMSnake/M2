#pragma once
#include <Wt/WContainerWidget.h>
#include "../materia/StrategyModel.hpp"
#include "../materia/FreeDataModel.hpp"

class IGoalViewCtrl;
class StrategyView : public Wt::WContainerWidget
{
public:
   StrategyView(StrategyModel& strategy, FreeDataModel& fd);

private:
   void onAddGoalClick();
   void onAddFreeDataBlock();
   void onAddWatchItemClick();
   void onBacklogClick();
   void layGoals();
   void fillDataBlocks(Wt::WToolBar& toolbar);
   
   void putGoal(const StrategyModel::Goal& goal);

   void refreshGoalCtrl(const materia::Id& id);

   StrategyModel& mStrategyModel;
   FreeDataModel& mFdModel;
   Wt::WToolBar* mMainToolbar;
   std::vector<IGoalViewCtrl*> mGoalCtrls;
};