#include "Strategy_v2.hpp"

namespace materia
{

Strategy_v2::Strategy_v2(IStrategy& strategy)
: mStrategy_v1(strategy)
{
   
}

void Strategy_v2::deleteObject(const Id& id)
{

}

Id Strategy_v2::addGoal(const Goal& goal)
{
   return Id::Invalid;
}

void Strategy_v2::modifyGoal(const Goal& goal)
{

}

std::vector<Goal> Strategy_v2::getGoals()
{
   return std::vector<Goal>();
}

std::optional<Goal> Strategy_v2::getGoal(const Id& id)
{
   return std::optional<Goal>();
}

std::optional<StrategyGraph> Strategy_v2::getGraph(const Id& id)
{
   return std::optional<StrategyGraph>();
}

Id Strategy_v2::createLink(const Id& nodeFrom, const Id& noteTo)
{
   return Id::Invalid;
}

Id Strategy_v2::createNode()
{
   return Id::Invalid;
}

}

