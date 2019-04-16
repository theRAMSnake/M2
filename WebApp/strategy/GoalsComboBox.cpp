#include "GoalsComboBox.hpp"

inline bool goalsSorter(const StrategyModel::Goal &a, const StrategyModel::Goal &b)
{
   if(a.focused == b.focused)
   {
      return a.title < b.title;
   }

   return a.focused > b.focused;
}

GoalComboBox::GoalComboBox(const std::vector<StrategyModel::Goal>& goals)
: mGoals(goals)
{
   std::sort(mGoals.begin(), mGoals.end(), goalsSorter);

   for(auto x : mGoals)
   {
      addItem(x.title);
   }
}

materia::Id GoalComboBox::getSelectedGoalId() const
{
   return mGoals[currentIndex()].id;
}

void GoalComboBox::select(const materia::Id& id)
{
   setCurrentIndex(std::distance(mGoals.begin(), materia::find_by_id(mGoals, id)));
}