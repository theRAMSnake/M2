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

StrategyModel::Objective StrategyModel::modifyObjective(const Objective& src)
{
   strategy::Objective o;
   o.mutable_common_props()->mutable_id()->set_guid(src.id);
   o.mutable_common_props()->set_name(src.title);
   o.set_reached(src.reached);
   o.mutable_parent_goal_id()->set_guid(src.parentGoalId);
   o.mutable_res_id()->set_guid(src.resId);
   o.set_expectedtreshold(src.expectedResValue);

   common::OperationResultMessage opResult;
   mService.getService().ModifyObjective(nullptr, &o, &opResult, nullptr);

   auto objs = getGoalObjectives(src.parentGoalId);
   auto pos = materia::find_by_id(objs, src.id);

   if(pos != objs.end())
   {
      return *pos;
   }
   else
   {
      throw -1;
   }
}

void StrategyModel::deleteTask(const materia::Id& src)
{
   common::UniqueId id;
   id.set_guid(src.getGuid());

   common::OperationResultMessage opResult;
   mService.getService().DeleteTask(nullptr, &id, &opResult, nullptr);
}

void StrategyModel::deleteObjective(const materia::Id& src)
{
   common::UniqueId id;
   id.set_guid(src.getGuid());

   common::OperationResultMessage opResult;
   mService.getService().DeleteObjective(nullptr, &id, &opResult, nullptr);
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

StrategyModel::Objective StrategyModel::addObjective(const std::string& title, const materia::Id& parentGoalId)
{
   common::UniqueId id;
   strategy::Objective o;

   o.mutable_common_props()->set_name(title);
   o.mutable_parent_goal_id()->set_guid(parentGoalId.getGuid());
   o.set_reached(false);

   mService.getService().AddObjective(nullptr, &o, &id, nullptr);
   
   return StrategyModel::Objective {id.guid(), title, false, parentGoalId};
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

std::vector<StrategyModel::Objective> StrategyModel::getGoalObjectives(const materia::Id& id)
{
   std::vector<Objective> result;

   common::UniqueId in;
   in.set_guid(id.getGuid());
   strategy::GoalItems items;

   mService.getService().GetGoalItems(nullptr, &in, &items, nullptr);

   for(auto o : items.objectives())
   {
      result.push_back(
         {
            o.common_props().id().guid(), 
            o.common_props().name(),
            o.reached(),
            o.parent_goal_id().guid(),
            o.res_id().guid(),
            o.expectedtreshold()
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

std::vector<StrategyModel::WatchItem> StrategyModel::getWatchItems()
{
   std::vector<WatchItem> result;

   common::EmptyMessage e;
   strategy::WatchItems res;

   mService.getService().GetWatchItems(nullptr, &e, &res, nullptr);

   for(auto r : res.items())
   {
      result.push_back({r.id().guid(), r.text()});
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

StrategyModel::WatchItem StrategyModel::addWatchItem(const std::string& name)
{
   strategy::WatchItemInfo itemToAdd;
   itemToAdd.set_text(name);

   common::UniqueId id;
   mService.getService().AddWatchItem(nullptr, &itemToAdd, &id, nullptr);

   return {id.guid(), name};
}
   
void StrategyModel::modifyWatchItem(const WatchItem& r)
{
   strategy::WatchItemInfo itemToEdit;
   itemToEdit.mutable_id()->set_guid(r.id.getGuid());
   itemToEdit.set_text(r.text);

   common::OperationResultMessage dummy;
   mService.getService().EditWatchItem(nullptr, &itemToEdit, &dummy, nullptr);
}

void StrategyModel::deleteWatchItem(const materia::Id& src)
{
   common::UniqueId id;
   id.set_guid(src.getGuid());

   common::OperationResultMessage opResult;
   mService.getService().DeleteWatchItem(nullptr, &id, &opResult, nullptr);
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

std::optional<StrategyModel::Resource> StrategyModel::getResource(const std::string& name)
{
   auto res = getResources();

   auto iter = std::find_if(res.begin(), res.end(), [&](auto x) -> bool {
      return x.name == name;
   });

   if(iter != res.end())
   {
      return *iter;
   }
   else
   {
      return std::optional<StrategyModel::Resource>();
   }
}

std::optional<StrategyModel::WatchItem> StrategyModel::getWatchItem(const materia::Id& id)
{
   auto items = getWatchItems();

   auto iter = materia::find_by_id(items, id);

   if(iter != items.end())
   {
      return *iter;
   }
   else
   {
      return std::optional<StrategyModel::WatchItem>();
   }
}

std::string StrategyModel::createDescriptiveTitle(const StrategyModel::Node& node)
{
   if(node.type == strategy::NodeType::GOAL)
   {
      return "Endpoint";
   }
   else if(node.type == strategy::NodeType::WATCH)
   {
      return "Wait for " + getWatchItem(node.watchItemReference)->text;
   }
   else if(!node.brief.empty())
   {
      return node.brief;
   }
   else
   {
      return "'" + node.id.getGuid() + "'";
   }
}

std::optional<StrategyModel::Graph> StrategyModel::getGraph(const materia::Id& src)
{
   common::UniqueId id;
   id.set_guid(src.getGuid());

   strategy::GraphDefinition def;

   mService.getService().GetGraph(nullptr, &id, &def, nullptr);

   if(def.nodes_size() > 0)
   {
      Graph g;

      for(auto x : def.nodes())
      {
         g.nodes.push_back({
            x.id().objectid().guid(), 
            x.node_type(), 
            "", 
            x.attrs().brief(),
            x.attrs().done(),
            {x.attrs().progress_current(), x.attrs().progress_total()},
            x.attrs().watch_item_reference().guid()
            });

         g.nodes.back().descriptiveTitle = createDescriptiveTitle(g.nodes.back());
      }

      for(auto x : def.links())
      {
         g.links.push_back({x.from_node_id().guid(), x.to_node_id().guid()});
      }

      return g;
   }
   else
   {
      return std::optional<StrategyModel::Graph>();
   }
}

void StrategyModel::createNode(const materia::Id& graphId)
{
   strategy::NodeProperties props;
   props.mutable_id()->mutable_graphid()->set_guid(graphId.getGuid());

   common::UniqueId id;
   mService.getService().CreateNode(nullptr, &props, &id, nullptr);
}

void StrategyModel::deleteNode(const materia::Id& graphId, const materia::Id& nodeId)
{
   strategy::GraphObjectId id;
   id.mutable_graphid()->set_guid(graphId.getGuid());
   id.mutable_objectid()->set_guid(nodeId.getGuid());

   common::OperationResultMessage result;
   mService.getService().DeleteNode(nullptr, &id, &result, nullptr);
}

void StrategyModel::createLink(const materia::Id& graphId, const materia::Id& fromNodeId, const materia::Id& toNodeId)
{
   strategy::LinkProperties id;
   id.mutable_graphid()->set_guid(graphId.getGuid());
   id.mutable_from_node_id()->set_guid(fromNodeId.getGuid());
   id.mutable_to_node_id()->set_guid(toNodeId.getGuid());

   common::EmptyMessage empty;
   mService.getService().CreateLink(nullptr, &id, &empty, nullptr);
}

void StrategyModel::deleteLink(const materia::Id& graphId, const materia::Id& fromNodeId, const materia::Id& toNodeId)
{
   strategy::LinkProperties id;
   id.mutable_graphid()->set_guid(graphId.getGuid());
   id.mutable_from_node_id()->set_guid(fromNodeId.getGuid());
   id.mutable_to_node_id()->set_guid(toNodeId.getGuid());

   common::OperationResultMessage opResult;
   mService.getService().DeleteLink(nullptr, &id, &opResult, nullptr);
}

void StrategyModel::updateNode(const materia::Id& graphId, const Node& node)
{
   strategy::NodeProperties props;
   props.mutable_id()->mutable_graphid()->set_guid(graphId.getGuid());
   props.mutable_id()->mutable_objectid()->set_guid(node.id.getGuid());
   props.set_node_type(node.type);

   auto attrs = props.mutable_attrs();
   attrs->set_done(node.isDone);
   attrs->set_brief(node.brief);
   attrs->set_progress_current(node.progress.first);
   attrs->set_progress_total(node.progress.second);
   if(node.watchItemReference != materia::Id::Invalid)
   {
      attrs->mutable_watch_item_reference()->set_guid(node.watchItemReference.getGuid());
   }

   common::OperationResultMessage opResult;
   mService.getService().ModifyNode(nullptr, &props, &opResult, nullptr);
}