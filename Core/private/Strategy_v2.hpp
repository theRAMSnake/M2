#pragma once

#include "../IStrategy_v2.hpp"
#include "../IStrategy.hpp"
#include "Database.hpp"

namespace materia
{

class Strategy_v2 : public IStrategy_v2
{
public:
   Strategy_v2(IStrategy& strategy, Database& db);

   Id addGoal(const Goal& goal) override;
   void modifyGoal(const Goal& goal) override;
   std::vector<Goal> getGoals() override;
   std::optional<Goal> getGoal(const Id& id) override;
   void deleteGoal(const Id& id) override;

   std::optional<StrategyGraph> getGraph(const Id& id) override; 

   void createLink(const Id& graphId, const Id& nodeFrom, const Id& nodeTo) override;
   void breakLink(const Id& graphId, const Id& nodeFrom, const Id& nodeTo) override;

   Id createNode(const Id& graphId) override;

   void deleteGraphObject(const Id& graphId, const Id& objectId) override;

private:
   void saveGraph(const StrategyGraph& graph);

   IStrategy& mStrategy_v1;
   std::unique_ptr<DatabaseTable> mGraphsStorage;
};

}