#pragma once

#include <Common/Id.hpp>
#include <vector>
#include "IStrategy.hpp"

namespace materia
{

struct Link
{
   Id from;
   Id to;
};

struct Node
{
   Id id;
};

struct StrategyGraph
{
   Id id;
   std::vector<Link> links;
   std::vector<Node> nodes;
};

class IStrategy_v2
{
public:
   virtual Id addGoal(const Goal& goal) = 0;
   virtual void modifyGoal(const Goal& goal) = 0;
   virtual std::vector<Goal> getGoals() = 0;
   virtual std::optional<Goal> getGoal(const Id& id) = 0;

   virtual std::optional<StrategyGraph> getGraph(const Id& id) = 0; 

   virtual Id createLink(const Id& from, const Id& to) = 0;

   virtual Id createNode() = 0;

   virtual void deleteObject(const Id& id) = 0;

   virtual ~IStrategy_v2(){}
};

}