#pragma once
#include <Wt/WContainerWidget.h>
#include "materia/RewardModel.hpp"

class RewardView : public Wt::WContainerWidget
{
public:
   RewardView(RewardModel& model);

private:
   void onAddPoolClick();
   void onAddPointsClick();

   void onItemDoubleClick(const RewardModel::Item& item);
   void onItemClick(Wt::WMouseEvent ev, const RewardModel::Item& item);

   void refreshList();

   Wt::WTable* mTable = nullptr;
   RewardModel& mModel;
};