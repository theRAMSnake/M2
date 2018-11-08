#include "StrategyModel.hpp"

StrategyModel::StrategyModel(ZmqPbChannel& channel)
: mService(channel)
{
   fetchGoals();
}

void StrategyModel::modifyTask(const Task& task)
{
   strategy::Task t;
   t.mutable_common_props()->mutable_id()->set_guid(task.id);
   t.mutable_common_props()->set_notes(task.notes);
   t.mutable_common_props()->set_name(task.title);
   t.mutable_parent_goal_id()->set_guid(task.parentGoalId);
   t.set_done(false);

   common::OperationResultMessage opResult;
   mService.getService().ModifyTask(nullptr, &t, &opResult, nullptr);

   *materia::find_by_id(mActiveTasks, task.id) = task;
}

void StrategyModel::deleteTask(const materia::Id& src)
{
   common::UniqueId id;
   id.set_guid(src.getGuid());

   common::OperationResultMessage opResult;
   mService.getService().DeleteTask(nullptr, &id, &opResult, nullptr);

   mActiveTasks.erase(materia::find_by_id(mActiveTasks, src));
}

std::vector<StrategyModel::Task> StrategyModel::getActiveTasks()
{
   return mActiveTasks;
}

void StrategyModel::fetchGoals()
{
   common::EmptyMessage e;
   strategy::Goals result;

   mService.getService().GetGoals(nullptr, &e, &result, nullptr);

   for(auto g : result.items())
   {
      mGoals.push_back(
         { 
            g.common_props().id().guid(), 
            g.common_props().name(),
            g.common_props().notes(),
            g.focused(),
            g.achieved()
         });

      if(g.focused())
      {
         strategy::GoalItems result;

         mService.getService().GetGoalItems(nullptr, &g.common_props().id(), &result, nullptr);

         for(auto t : result.tasks())
         {
            mActiveTasks.push_back(
               {
                  t.common_props().id().guid(), 
                  t.common_props().name(),
                  t.common_props().notes(),
                  t.parent_goal_id().guid()
               });
         }
      }
   }
}

std::vector<StrategyModel::Goal> StrategyModel::getGoals()
{
   return mGoals;
}

const StrategyModel::Goal& StrategyModel::addGoal(const bool focused, const std::string& name)
{
   common::UniqueId id;
   strategy::Goal g;

   g.mutable_common_props()->set_name(name);
   g.set_focused(focused);

   mService.getService().AddGoal(nullptr, &g, &id, nullptr);
   mService.getService().GetGoal(nullptr, &id, &g, nullptr);

   mGoals.push_back(
      { 
         g.common_props().id().guid(), 
         g.common_props().name(),
         g.common_props().notes(),
         g.focused(),
         g.achieved()
      });

   return mGoals.back();
}