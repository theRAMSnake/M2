#pragma once

#include "../IStrategy_v2.hpp"
#include "../IStrategy.hpp"
#include "Database.hpp"

namespace materia
{

class Strategy_v2 : public IStrategy_v2
{
public:
   Strategy_v2(IStrategy& strategy);

   Id addGoal(const Goal& goal) override;
   void modifyGoal(const Goal& goal) override;
   std::vector<Goal> getGoals() override;
   std::optional<Goal> getGoal(const Id& id) override;

   std::optional<StrategyGraph> getGraph(const Id& id) override; 

   Id createLink(const Id& nodeFrom, const Id& noteTo) override;

   Id createNode() override;

   void deleteObject(const Id& id) override;

private:
   IStrategy& mStrategy_v1;
};

}