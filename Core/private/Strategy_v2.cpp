#include "Strategy_v2.hpp"
#include "JsonSerializer.hpp"
#include "Common/Utils.hpp"
#include "Logger.hpp"

SERIALIZE_AS_INTEGER(materia::NodeType)
SERIALIZE_AS_INTEGER(materia::NodeAttributeType)

BIND_JSON5(materia::Goal, id, name, notes, focused, achieved)
BIND_JSON4(materia::RawStrategyGraph, id, nodes, links, nodeAttrs)
BIND_JSON2(materia::Node, id, type)
BIND_JSON2(materia::Link, from, to)
BIND_JSON3(materia::FocusItem, id, graphId, nodeId)

using attr_map = std::pair<materia::Id, std::map<materia::NodeAttributeType, std::string>>; 
BIND_JSON2(attr_map, first, second)

using attr_map_node = std::pair<materia::NodeAttributeType, std::string>; 
BIND_JSON2(attr_map_node, first, second)

namespace materia
{

Strategy_v2::Strategy_v2(Database& db)
: mGraphsStorage(db.getTable("graphs"))
, mWatchStorage(db.getTable("watchItems"))
, mFocusStorage(db.getTable("focusItems"))
, mGoalsStorage(db.getTable("goals"))
{
   LOG("Start strategy2 init");
}

std::shared_ptr<StrategyGraph> Strategy_v2::loadGraph(const Id& graphId)
{
   auto loadedJson = mGraphsStorage->load(graphId);

   if(loadedJson)
   {
      auto result = std::make_shared<StrategyGraph>(readJson<RawStrategyGraph>(*loadedJson));
      updateCompleteness(*result);

      return result;
   }
   else
   {
      return std::shared_ptr<StrategyGraph>();
   }
}

std::vector<Node> getPredecessors(const StrategyGraph& g, const Id sucessorId)
{
   std::vector<Node> result;

   for(auto l : g.getLinks())
   {
      if(l.to == sucessorId)
      {
         auto srcNode = find_by_id(g.getNodes(), l.from);
         if(srcNode != g.getNodes().end())
         {
            result.push_back(*srcNode);
         }
      }
   }

   return result;
}

bool Strategy_v2::getNodeSelfCompleteness(const StrategyGraph& graph, const Node& node)
{
   auto attrs = graph.getNodeAttributes(node.id);

   switch (node.type)
   {
   case NodeType::Blank:
      return false;

   case NodeType::Counter:
      return attrs.get<NodeAttributeType::PROGRESS_CURRENT>() >=
            attrs.get<NodeAttributeType::PROGRESS_TOTAL>();

   case NodeType::Goal:
      return !getPredecessors(graph, node.id).empty();

   case NodeType::Reference:
      {
         auto g = getGoal(attrs.get<NodeAttributeType::GOAL_REFERENCE>());
         return g && g->achieved; 
      }
      
   case NodeType::Task:
      return attrs.get<NodeAttributeType::IS_DONE>();

   case NodeType::Wait:
      return attrs.get<NodeAttributeType::REQUIRED_TIMESTAMP>() < std::time(0);

   case NodeType::Watch:
      {
         auto w = getWatchItems();
         return find_by_id(w, attrs.get<NodeAttributeType::WATCH_ITEM_REFERENCE>()) == w.end();
      }
   }

   return false;
}

void Strategy_v2::updateNodeCompleteness(const StrategyGraph& graph, const Node& node, std::map<Id, bool>& completenessPerNode)
{
   auto nodes = graph.getNodes();
   auto links = graph.getLinks();

   auto predecessors = getPredecessors(graph, node.id);

   bool nodeCompleted = true;

   for(auto p : predecessors)
   {
      auto prevResult = completenessPerNode.find(p.id);

      if(prevResult == completenessPerNode.end())
      {
         updateNodeCompleteness(graph, p, completenessPerNode);
         prevResult = completenessPerNode.find(p.id);
      }
      
      nodeCompleted = nodeCompleted && prevResult->second;
   }

   nodeCompleted = nodeCompleted && getNodeSelfCompleteness(graph, node);

   completenessPerNode[node.id] = nodeCompleted;
}

void Strategy_v2::updateCompleteness(StrategyGraph& graph)
{
   auto nodes = graph.getNodes();
   auto links = graph.getLinks();

   //Find all endpoint nodes
   std::vector<Node> endpointNodes;
   std::copy_if(nodes.begin(), nodes.end(), std::back_inserter(endpointNodes), [&](auto x)
   {
      //something leads to it
      //nothing lead from it
      return links.end() != std::find_if(links.begin(), links.end(), [&](auto l){return l.to == x.id;}) &&
         links.end() == std::find_if(links.begin(), links.end(), [&](auto l){return l.from == x.id;});
   });

   //copy unlinked nodes
   std::copy_if(nodes.begin(), nodes.end(), std::back_inserter(endpointNodes), [&](auto x)
   {
      return links.end() == std::find_if(links.begin(), links.end(), [&](auto l){return l.to == x.id || l.from == x.id;});
   });

   std::map<Id, bool> completenessPerNode;
   for(auto x : endpointNodes)
   {
      updateNodeCompleteness(graph, x, completenessPerNode);
   }

   assert(completenessPerNode.size() == nodes.size());

   for(auto x : completenessPerNode)
   {
      auto attrs = graph.getNodeAttributes(x.first);
      attrs.set<NodeAttributeType::IS_DONE>(x.second);
      graph.setNodeAttributes(x.first, attrs);
   }

   bool isGoalNodeAchieved = graph.getNodeAttributes(graph.getGoalNodeId()).get<NodeAttributeType::IS_DONE>();
   auto goal = *getGoal(graph.getId());
   if(goal.achieved != isGoalNodeAchieved)
   {
      goal.achieved = isGoalNodeAchieved;
      modifyGoal(goal);
   }
}

void Strategy_v2::deleteNode(const Id& graphId, const Id& objectId)
{
   makeGraphOperation(graphId, [=](auto& g){g.deleteNode(objectId);});
}

Id Strategy_v2::addGoal(const Goal& goal)
{
   auto g = goal;
   g.id = Id::generate();
   g.achieved = false;

   std::string json = writeJson(g);
   mGoalsStorage->store(g.id, json);

   saveGraph(*std::make_shared<StrategyGraph>(g.id));

   return g.id;
}

void Strategy_v2::modifyGoal(const Goal& goal)
{
   std::string json = writeJson(goal);
   mGoalsStorage->store(goal.id, json);
}

std::vector<Goal> Strategy_v2::getGoals()
{
   std::vector<Goal> result;

   mGoalsStorage->foreach([&](std::string id, std::string json) 
   {
      result.push_back(readJson<Goal>(json));
   });

   return result;
}

std::optional<Goal> Strategy_v2::getGoal(const Id& id)
{
   auto goals = getGoals();
   auto pos = find_by_id(goals, id);
   if(pos != goals.end())
   {
       return *pos;
   }

   return std::optional<Goal>();
}

std::shared_ptr<IStrategyGraph> Strategy_v2::getGraph(const Id& id)
{
   return loadGraph(id);
}

void Strategy_v2::createLink(const Id& graphId, const Id& nodeFrom, const Id& nodeTo)
{
   makeGraphOperation(graphId, [=](auto& g){g.createLink(nodeFrom, nodeTo);});
}

void Strategy_v2::breakLink(const Id& graphId, const Id& nodeFrom, const Id& nodeTo)
{
   makeGraphOperation(graphId, [=](auto& g){g.breakLink(nodeFrom, nodeTo);});
}

Id Strategy_v2::createNode(const Id& graphId)
{
   auto result = Id::Invalid;

   makeGraphOperation(graphId, [&](auto& g){result = g.createNode();});

   return result;
}

void Strategy_v2::setNodeAttributes(const Id& graphId, const Id& objectId, const NodeType& nodeType, const NodeAttributes& attrs)
{
   makeGraphOperation(graphId, [=](auto& g)
   {
      g.setNodeAttributes(objectId, nodeType, attrs);
   });
}

void Strategy_v2::deleteGoal(const Id& id)
{
   mGoalsStorage->erase(id);
   mGraphsStorage->erase(id);
}

void Strategy_v2::saveGraph(const StrategyGraph& graph)
{
   auto& rawData = graph.getRawData();
   std::string json = writeJson(rawData);
   //std::cout << json;
   mGraphsStorage->store(rawData.id, json);
}

std::vector<WatchItem> Strategy_v2::getWatchItems() const
{
   std::vector<WatchItem> result;

   mWatchStorage->foreach([&](std::string id, std::string json) 
   {
      result.push_back({id, json});
   });

   return result;
}

void Strategy_v2::removeWatchItem(const Id& id)
{
   mWatchStorage->erase(id);
}

Id Strategy_v2::addWatchItem(const WatchItem& item)
{
   auto newItem = item;
   newItem.id = Id::generate();

   mWatchStorage->store(newItem.id, item.text);

   return newItem.id;
}

void Strategy_v2::replaceWatchItem(const WatchItem& item)
{
   mWatchStorage->store(item.id, item.text);
}

std::vector<FocusItem> Strategy_v2::getFocusItems() const
{
   std::vector<FocusItem> result;

   mFocusStorage->foreach([&](std::string id, std::string json) 
   {
      result.push_back(readJson<FocusItem>(json));
   });

   return result;
}

void Strategy_v2::removeFocusItem(const Id& id)
{
   mFocusStorage->erase(id);
}

void Strategy_v2::completeFocusItem(const FocusItem& item)
{
   mFocusStorage->erase(item.id);

   makeGraphOperation(item.graphId, [=](auto& g)
   {
      auto node = g.getNode(item.nodeId);

      if(node)
      {
         NodeAttributes attrs = g.getNodeAttributes(item.nodeId);

         switch(node->type)
         {
            case NodeType::Task:
            {
               attrs.set<NodeAttributeType::IS_DONE>(true);
            }  
            break;

            case NodeType::Counter:
            {
               auto val = attrs.get<NodeAttributeType::PROGRESS_CURRENT>();
               attrs.set<NodeAttributeType::PROGRESS_CURRENT>(val + 1);
            } 
            break;

            default:
               LOG_WARNING("Completion of focus item for node type of " + std::to_string(static_cast<int>(node->type)) + " is not supported");
               return;
         }

         g.setNodeAttributes(item.nodeId, node->type, attrs);
      }
   });
}

Id Strategy_v2::addFocusItem(const FocusItem& item)
{
   auto newItem = item;
   newItem.id = Id::generate();

   std::string json = writeJson(newItem);
   mFocusStorage->store(newItem.id, json);

   return newItem.id;
}

}

