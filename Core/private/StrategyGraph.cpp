#include "StrategyGraph.hpp"
#include <Common/Utils.hpp>

namespace materia
{

bool operator == (const materia::Link& a, const materia::Link& b)
{
   return a.from == b.from && a.to == b.to;
}

StrategyGraph::StrategyGraph(const RawStrategyGraph& src)
: mSrc(src)
{

}

std::vector<Link> StrategyGraph::getLinks() const
{
   return mSrc.links;
}

std::vector<Node> StrategyGraph::getNodes() const
{
   return mSrc.nodes;
}

SimpleNodeAttributes StrategyGraph::getSimpleNodeAttributes(const Id& nodeId) const
{
   return mSrc.simpleNodeAttrs.find(nodeId)->second;
}

CounterNodeAttributes StrategyGraph::getCounterNodeAttributes(const Id& nodeId) const
{
   return mSrc.counterNodeAttrs.find(nodeId)->second;
}

bool StrategyGraph::containsLinkAnyDirection(const Id& nodeFrom, const Id& nodeTo) const
{
   return mSrc.links.end() != std::find_if(mSrc.links.begin(), mSrc.links.end(), [&](auto x)->bool 
   {
      return (x.from == nodeFrom && x.to == nodeTo) ||
         (x.to == nodeFrom && x.from == nodeTo);
   });
}

bool StrategyGraph::isRouteExist(const Id& from, const Id& destination) const
{
   if(from == destination)
   {
      return true;
   }
   
   std::vector<Link> allOutgoingLinks;
   std::copy_if(mSrc.links.begin(), mSrc.links.end(), std::back_inserter(allOutgoingLinks), [&](auto l){return l.from == from;});

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
      if(isRouteExist(x.to, destination))
      {
         return true;
      }
   }
   
   return false;
}

Node StrategyGraph::getGoalNode() const
{
    return *std::find_if(mSrc.nodes.begin(), mSrc.nodes.end(), [&](auto x)->bool {return x.type == NodeType::Goal;});
}

void StrategyGraph::deleteNode(const Id& objectId)
{
   auto pos = find_by_id(mSrc.nodes, objectId);
   if(pos != mSrc.nodes.end() && pos->type != NodeType::Goal)
   {
      mSrc.nodes.erase(pos);
      erase_if(mSrc.links, [objectId](auto x) {return x.from == objectId || x.to == objectId;});
      mSrc.simpleNodeAttrs.erase(objectId);
      mSrc.counterNodeAttrs.erase(objectId);
   }
}

StrategyGraph::StrategyGraph(const Id& id)
{
   mSrc.id = id;
   mSrc.nodes.push_back({Id::generate(), NodeType::Goal});
}

void StrategyGraph::createLink(const Id& from, const Id& to)
{
   if(from == to)
   {
      return;
   }

   auto& nodes = mSrc.nodes;
   
   if(contains_id(nodes, from) && 
      contains_id(nodes, to) && 
      !containsLinkAnyDirection(from, to) &&
      from != getGoalNode().id)
   {
      if(!isRouteExist(to, from))
      {
         mSrc.links.push_back({from, to});
      }
   }
}

void StrategyGraph::breakLink(const Id& nodeFrom, const Id& nodeTo)
{
   erase_if(mSrc.links, [=](auto l){return l.from == nodeFrom && l.to == nodeTo;});
}

Id StrategyGraph::createNode()
{
   auto result = Id::generate();
   mSrc.nodes.push_back({result, NodeType::Blank});
   return result;
}

const RawStrategyGraph& StrategyGraph::getRawData() const
{
   return mSrc;
}

}