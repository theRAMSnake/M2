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

void StrategyModel::deleteGoal(const materia::Id& src)
{
   common::UniqueId id;
   id.set_guid(src.getGuid());

   common::OperationResultMessage opResult;
   mService.getService().DeleteGoal(nullptr, &id, &opResult, nullptr);

   mGoals.erase(materia::find_by_id(mGoals, src));
}

std::vector<StrategyModel::Task> StrategyModel::getActiveTasks()
{
   std::vector<StrategyModel::Task> result;

   for(auto g : mGoals)
   {
      if(g.focused)
      {
         auto goalTasks = getGoalTasks(g.id);
         result.insert(result.end(), goalTasks.begin(), goalTasks.end());
      }
   }

   return result;
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

void StrategyModel::modifyGoal(const Goal& goal)
{
   auto iter = materia::find_by_id(mGoals, goal.id);

   if(iter != mGoals.end())
   {
      *iter = goal;

      strategy::Goal g;
      common::OperationResultMessage op;

      g.mutable_common_props()->set_name(goal.title);
      g.mutable_common_props()->mutable_id()->set_guid(goal.id.getGuid());
      g.mutable_common_props()->set_notes(goal.notes);
      g.set_focused(goal.focused);

      mService.getService().ModifyGoal(nullptr, &g, &op, nullptr);
   }
}

StrategyModel::Task StrategyModel::addTask(const std::string& title, const materia::Id& parentGoalId)
{
   common::UniqueId id;
   strategy::Task t;

   t.mutable_common_props()->set_name(title);
   t.mutable_parent_goal_id()->set_guid(parentGoalId.getGuid());

   mService.getService().AddTask(nullptr, &t, &id, nullptr);
   
   return StrategyModel::Task {id.guid(), title, "", parentGoalId};
}

std::vector<StrategyModel::Task> StrategyModel::getGoalTasks(const materia::Id& id)
{
   std::vector<Task> result;

   common::UniqueId in;
   in.set_guid(id.getGuid());
   strategy::GoalItems items;

   mService.getService().GetGoalItems(nullptr, &in, &items, nullptr);

   for(auto t : items.tasks())
   {
      result.push_back(
         {
            t.common_props().id().guid(), 
            t.common_props().name(),
            t.common_props().notes(),
            t.parent_goal_id().guid()
         });
   }

   return result;
}

std::vector<StrategyModel::Resource> StrategyModel::getResources()
{
   std::vector<Resource> result;

   common::EmptyMessage e;
   strategy::Resources res;

   mService.getService().GetResources(nullptr, &e, &res, nullptr);

   for(auto r : res.items())
   {
      result.push_back(Resource{r.id().guid(), r.name(), r.value()});
   }

   return result;
}

StrategyModel::Resource StrategyModel::addResource(const std::string& name)
{
   common::UniqueId id;
   strategy::Resource r;

   r.set_name(name);
   r.set_value(0);

   mService.getService().AddResource(nullptr, &r, &id, nullptr);
   
   return StrategyModel::Resource {id.guid(), name, 0};
}

void StrategyModel::deleteResource(const materia::Id& src)
{
   common::UniqueId id;
   id.set_guid(src.getGuid());

   common::OperationResultMessage opResult;
   mService.getService().DeleteResource(nullptr, &id, &opResult, nullptr);
}

void StrategyModel::modifyResource(const Resource& r)
{
   strategy::Resource res;
   res.set_name(r.name);
   res.set_value(r.value);
   res.mutable_id()->set_guid(r.id.getGuid());

   common::OperationResultMessage opResult;
   mService.getService().ModifyResource(nullptr, &res, &opResult, nullptr);
}