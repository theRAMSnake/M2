#pragma once

#include "../IStrategy_v2.hpp"

namespace materia
{

struct RawStrategyGraph
{
   Id id;
   std::vector<Link> links;
   std::vector<Node> nodes;

   std::map<Id, SimpleNodeAttributes> simpleNodeAttrs;
   std::map<Id, CounterNodeAttributes> counterNodeAttrs;
};

class StrategyGraph : public IStrategyGraph
{
public:
   StrategyGraph(const Id& id);
   StrategyGraph(const RawStrategyGraph& src);

   std::vector<Link> getLinks() const override;
   std::vector<Node> getNodes() const override;

   SimpleNodeAttributes getSimpleNodeAttributes(const Id& nodeId) const override;
   CounterNodeAttributes getCounterNodeAttributes(const Id& nodeId) const override;

   void createLink(const Id& from, const Id& to);
   void breakLink(const Id& nodeFrom, const Id& nodeTo);

   Id createNode();
   void deleteNode(const Id& objectId);

   void setNodeAttributes(const Id& objectId, const NodeType& nodeType, const SimpleNodeAttributes& attrs);
   void setNodeAttributes(const Id& objectId, const CounterNodeAttributes& attrs);

   const RawStrategyGraph& getRawData() const;

private:

   void discardAttributes(const Id& objectId);
   bool containsLinkAnyDirection(const Id& nodeFrom, const Id& nodeTo) const;
   bool isRouteExist(const Id& from, const Id& destination) const;
   Node getGoalNode() const;

   RawStrategyGraph mSrc;
};

}