#pragma once

#include <Wt/WComboBox.h>
#include "materia/StrategyModel.hpp"

class GoalComboBox : public Wt::WComboBox
{
public:
   GoalComboBox(const std::vector<StrategyModel::Goal>& goals);

   materia::Id getSelectedGoalId() const;
   void select(const materia::Id& id);

private:
   std::vector<StrategyModel::Goal> mGoals;
};