#include "Strategy_v2.hpp"
#include "JsonSerializer.hpp"

SERIALIZE_AS_INTEGER(materia::NodeType)

BIND_JSON5(materia::RawStrategyGraph, id, nodes, links, simpleNodeAttrs, counterNodeAttrs)
BIND_JSON2(materia::Node, id, type)

using TSimpleAttrs = std::pair<materia::Id, materia::SimpleNodeAttributes>;
BIND_JSON3(TSimpleAttrs, first, second.brief, second.done)

using TCounterAttrs = std::pair<materia::Id, materia::CounterNodeAttributes>;
BIND_JSON4(TCounterAttrs, first, second.brief, second.current, second.required)

BIND_JSON2(materia::Link, from, to)

namespace materia
{

Strategy_v2::Strategy_v2(IStrategy& strategy, Database& db)
: mStrategy_v1(strategy)
, mGraphsStorage(db.getTable("graphs"))
{
   
}

std::shared_ptr<StrategyGraph> Strategy_v2::loadGraph(const Id& graphId)
{
   auto loaded = mGraphsStorage->load(graphId);

   if(loaded)
   {
      //std::cout << "LLL " << *loaded;
      return std::make_shared<StrategyGraph>(readJson<RawStrategyGraph>(*loaded));
   }
   else
   {
      return std::shared_ptr<StrategyGraph>();
   }
}

void Strategy_v2::deleteNode(const Id& graphId, const Id& objectId)
{
   makeGraphOperation(graphId, [=](auto& g){g.deleteNode(objectId);});
}

Id Strategy_v2::addGoal(const Goal& goal)
{
   auto id = mStrategy_v1.addGoal(goal);

   saveGraph(*std::make_shared<StrategyGraph>(id));

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

void Strategy_v2::setNodeAttributes(const Id& graphId, const Id& objectId, const NodeType& nodeType, const SimpleNodeAttributes& attrs)
{
   makeGraphOperation(graphId, [=](auto& g)
   {
      g.setNodeAttributes(objectId, nodeType, attrs);
   });
}

void Strategy_v2::setNodeAttributes(const Id& graphId, const Id& objectId, const CounterNodeAttributes& attrs)
{
   makeGraphOperation(graphId, [=](auto& g)
   {
      g.setNodeAttributes(objectId, attrs);
   });
}

void Strategy_v2::deleteGoal(const Id& id)
{
   mStrategy_v1.deleteGoal(id);
   mGraphsStorage->erase(id);
}

void Strategy_v2::saveGraph(const StrategyGraph& graph)
{
   auto& rawData = graph.getRawData();
   std::string json = writeJson(rawData);
   //std::cout << json;
   mGraphsStorage->store(rawData.id, json);
}

}

