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

NodeAttributes StrategyGraph::getNodeAttributes(const Id& nodeId) const
{
   return NodeAttributes(mSrc.nodeAttrs.find(nodeId)->second);
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
      mSrc.nodeAttrs.erase(objectId);
   }
}

StrategyGraph::StrategyGraph(const Id& id)
{
   mSrc.id = id;
   mSrc.nodes.push_back({Id::generate(), NodeType::Goal});
   mSrc.nodeAttrs[mSrc.nodes.back().id] = {};
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
   mSrc.nodeAttrs[mSrc.nodes.back().id] = {};
   return result;
}

const RawStrategyGraph& StrategyGraph::getRawData() const
{
   return mSrc;
}

const std::map<NodeType, std::vector<NodeAttributeType>> REQUIRED_ATTRIBUTES_PER_NODE_TYPE =
   {
      {NodeType::Goal, {}},
      {NodeType::Blank, {}},
      {NodeType::Counter, {NodeAttributeType::PROGRESS_CURRENT, NodeAttributeType::PROGRESS_TOTAL, NodeAttributeType::BRIEF}},
      {NodeType::Task, {NodeAttributeType::BRIEF}},
      {NodeType::Reference, {NodeAttributeType::GRAPH_REFERENCE}},
      {NodeType::Wait, {NodeAttributeType::REQUIRED_TIMESTAMP}},
      {NodeType::Watch, {NodeAttributeType::WATCH_ITEM_REFERENCE}},
   }; 

void StrategyGraph::setNodeAttributes(const Id& objectId, const NodeType& type, const NodeAttributes& attrs)
{
   auto nodePos = find_by_id(mSrc.nodes, objectId);

   if(nodePos != mSrc.nodes.end() && nodePos->type != NodeType::Goal)
   {
      NodeAttributes modifiedAttrs = attrs;
      nodePos->type = type;

      if(type == NodeType::Counter)
      {
         modifiedAttrs.set<NodeAttributeType::IS_DONE>(modifiedAttrs.get<NodeAttributeType::PROGRESS_CURRENT>() >=
            modifiedAttrs.get<NodeAttributeType::PROGRESS_TOTAL>());
      }
      
      auto& reqAttrTypes = REQUIRED_ATTRIBUTES_PER_NODE_TYPE.find(type)->second;

      for(auto x : reqAttrTypes)
      {
         if(!attrs.contains(x))
         {
            return;
         }
      }

      mSrc.nodeAttrs[objectId] = modifiedAttrs.toMap();
   }
}

}