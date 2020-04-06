#include "StrategyModel.hpp"

StrategyModel::StrategyModel(ZmqPbChannel& channel, ChallengeModel& chModel)
: mService(channel)
, mChModel(chModel)
{
   fetchGoals();
}

void StrategyModel::deleteGoal(const materia::Id& src)
{
   common::UniqueId id;
   id.set_guid(src.getGuid());

   common::OperationResultMessage opResult;
   mService.getService().DeleteGoal(nullptr, &id, &opResult, nullptr);

   mGoals.erase(materia::find_by_id(mGoals, src));
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
   itemToEdit.set_text(r.title);

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
      return "Wait for " + getWatchItem(node.watchItemReference)->title;
   }
   else if(node.type == strategy::NodeType::REFERENCE)
   {
      auto pos = materia::find_by_id(mGoals, node.graphReference);
      if(pos == mGoals.end())
      {
         return "Wait for '" + node.graphReference.getGuid() + "'";
      }
      else
      {
         return "Wait for " + pos->title;
      }
   }
   else if(node.type == strategy::NodeType::CHALLENGE)
   {
      auto chs = mChModel.get();
      auto pos = materia::find_by_id(chs, node.challengeReference);
      if(pos == chs.end())
      {
         return "Wait for '" + node.challengeReference.getGuid() + "'";
      }
      else
      {
         return "Wait for " + pos->title;
      }
   }
   else if(node.type == strategy::NodeType::WAIT)
   {
      char buffer[256];
      auto tm = std::localtime(&node.requiredTimestamp);
      strftime(buffer, sizeof(buffer), "%d %b %Y", tm);

      return "Wait till " + std::string(buffer);
   }
   else if(node.type == strategy::NodeType::CONDITION)
   {
      return node.condition;
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
            x.attrs().watch_item_reference().guid(),
            x.attrs().graph_reference().guid(),
            x.attrs().required_timestamp(),
            x.attrs().fd_expression(),
            x.attrs().challenge_reference().guid()
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

materia::Id StrategyModel::createNode(const materia::Id& graphId)
{
   strategy::NodeProperties props;
   props.mutable_id()->mutable_graphid()->set_guid(graphId.getGuid());

   common::UniqueId id;
   mService.getService().CreateNode(nullptr, &props, &id, nullptr);

   return materia::Id(id.guid());
}

materia::Id StrategyModel::cloneNode(const materia::Id& graphId, const Node& node)
{
   auto newNodeId = createNode(graphId);

   auto clonedNode = node;
   clonedNode.id = newNodeId;

   updateNode(graphId, clonedNode);

   return newNodeId;
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
   attrs->mutable_watch_item_reference()->set_guid(node.watchItemReference.getGuid());
   attrs->mutable_graph_reference()->set_guid(node.graphReference.getGuid());
   attrs->set_required_timestamp(node.requiredTimestamp);
   attrs->set_fd_expression(node.condition);
   attrs->mutable_challenge_reference()->set_guid(node.challengeReference.getGuid());

   common::OperationResultMessage opResult;
   mService.getService().ModifyNode(nullptr, &props, &opResult, nullptr);
}

std::optional<StrategyModel::Goal> StrategyModel::getGoal(const materia::Id& id)
{
   auto pos = materia::find_by_id(mGoals, id);
   if(pos != mGoals.end())
   {
      return *pos;
   }
   else
   {
      return std::optional<StrategyModel::Goal>();
   }
}

void StrategyModel::splitNodeVertical(const materia::Id& graphId, const Node& node)
{
   auto newNodeId = cloneNode(graphId, node);

   auto g = getGraph(graphId);
   std::vector<StrategyModel::Link> inboundLinks;
   std::copy_if(g->links.begin(), g->links.end(), std::back_inserter(inboundLinks), [&](auto l)
   {
      return l.to == node.id;
   });

   std::vector<StrategyModel::Link> outboundLinks;
   std::copy_if(g->links.begin(), g->links.end(), std::back_inserter(outboundLinks), [&](auto l)
   {
      return l.from == node.id;
   });

   for(auto l : inboundLinks)
   {
      createLink(graphId, l.from, newNodeId);
   }

   for(auto l : outboundLinks)
   {
      createLink(graphId, newNodeId, l.to);
   }
}

void StrategyModel::splitNodeHorizontal(const materia::Id& graphId, const Node& node)
{
   auto newNodeId = cloneNode(graphId, node);

   auto g = getGraph(graphId);
   std::vector<StrategyModel::Link> outboundLinks;
   std::copy_if(g->links.begin(), g->links.end(), std::back_inserter(outboundLinks), [&](auto l)
   {
      return l.from == node.id;
   });

   for(auto l : outboundLinks)
   {
      deleteLink(graphId, l.from, l.to);
   }

   for(auto l : outboundLinks)
   {
      createLink(graphId, newNodeId, l.to);
   }

   createLink(graphId, node.id, newNodeId);
}