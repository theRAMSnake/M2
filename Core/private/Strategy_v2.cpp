#include "Strategy_v2.hpp"
#include "JsonSerializer.hpp"

BIND_JSON3(materia::StrategyGraph, id, nodes, links)
BIND_JSON1(materia::Node, id)
BIND_JSON2(materia::Link, from, to)

namespace materia
{

Strategy_v2::Strategy_v2(IStrategy& strategy, Database& db)
: mStrategy_v1(strategy)
, mGraphsStorage(db.getTable("graphs"))
{
   
}

void Strategy_v2::deleteGraphObject(const Id& graphId, const Id& objectId)
{

}

Id Strategy_v2::addGoal(const Goal& goal)
{
   auto id = mStrategy_v1.addGoal(goal);

   StrategyGraph newGraph { id };

   saveGraph(newGraph);

   return id;
}

void Strategy_v2::modifyGoal(const Goal& goal)
{
   mStrategy_v1.modifyGoal(goal);
}

std::vector<Goal> Strategy_v2::getGoals()
{
   return mStrategy_v1.getGoals();
}

std::optional<Goal> Strategy_v2::getGoal(const Id& id)
{
   return mStrategy_v1.getGoal(id);
}

std::optional<StrategyGraph> Strategy_v2::getGraph(const Id& id)
{
   auto loaded = mGraphsStorage->load(id);

   if(loaded)
   {
      //std::cout << "LLL " << *loaded;
      return readJson<StrategyGraph>(*loaded);
   }
   else
   {
      return std::optional<StrategyGraph>();
   }
}

bool contains_link(const std::vector<Link>& links, const Id& nodeFrom, const Id& nodeTo)
{
   return links.end() != std::find_if(links.begin(), links.end(), [&](auto x)->bool 
   {
      return (x.from == nodeFrom && x.to == nodeTo) ||
         (x.to == nodeFrom && x.from == nodeTo);
   });
}

bool isRouteExist(const std::vector<Link>& links, const Id& from, const Id& destination)
{
   if(from == destination)
   {
      return true;
   }
   
   std::vector<Link> allOutgoingLinks;
   std::copy_if(links.begin(), links.end(), std::back_inserter(allOutgoingLinks), [&](auto l){return l.from == from;});

   if(std::find_if(allOutgoingLinks.begin(), allOutgoingLinks.end(), [&](auto l){return l.to == destination;}) 
      != allOutgoingLinks.end())
   {
      return true;
   }

   //check every link this link can route to
   //if one of them can route to destination - return true
   //else return false
   for(auto x : allOutgoingLinks)
   {
      if(isRouteExist(links, x.to, destination))
      {
         return true;
      }
   }
   
   return false;
}

void Strategy_v2::createLink(const Id& graphId, const Id& nodeFrom, const Id& nodeTo)
{
   if(nodeFrom == nodeTo)
   {
      return;
   }

   auto graph = getGraph(graphId);

   if(graph)
   {
      auto& nodes = graph->nodes;
      auto& links = graph->links;

      if(contains_id(nodes, nodeFrom) && contains_id(nodes, nodeTo) && 
         !contains_link(links, nodeFrom, nodeTo))
      {
         links.push_back({nodeFrom, nodeTo});

         if(!isRouteExist(links, nodeTo, nodeFrom))
         {
            saveGraph(*graph);
         }
      }
   }
}

void Strategy_v2::breakLink(const Id& graphId, const Id& nodeFrom, const Id& noteTo)
{
   
}

Id Strategy_v2::createNode(const Id& graphId)
{
   auto result = Id::Invalid;
   auto graph = getGraph(graphId);

   if(graph)
   {
      result = Id::generate();
      graph->nodes.push_back({result});

      saveGraph(*graph);
   }

   return result;
}

void Strategy_v2::deleteGoal(const Id& id)
{
   mStrategy_v1.deleteGoal(id);
   mGraphsStorage->erase(id);
}

void Strategy_v2::saveGraph(const StrategyGraph& graph)
{
   std::string json = writeJson(graph);
   //std::cout << json;
   mGraphsStorage->store(graph.id, json);
}

}

