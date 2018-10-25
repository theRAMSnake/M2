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
}

void StrategyModel::deleteTask(const materia::Id& src)
{
   common::UniqueId id;
   id.set_guid(src.getGuid());

   common::OperationResultMessage opResult;
   mService.getService().DeleteTask(nullptr, &id, &opResult, nullptr);
}

std::vector<StrategyModel::Task> StrategyModel::getActiveTasks()
{

}

void StrategyModel::fetchGoals()
{
   common::EmptyMessage e;
   strategy::Goals result;

   mService.getService().GetGoals(nullptr, &e, &g, nullptr);

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

      //fetch items if focused
   }
}