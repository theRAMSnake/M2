#pragma once

#include "../IStrategy_v2.hpp"
#include "../IStrategy.hpp"
#include "Database.hpp"

namespace materia
{

struct Node
{
   Id id;
   NodeType type;
   std::string brief;
};

struct StrategyGraph
{
   Id id;
   std::vector<Link> links;
   std::vector<Node> nodes;
};

struct Link
{
   Id from;
   Id to;
};

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
   void setNodeAttributes(const Id& graphId, const Node& node) override;

   void deleteNode(const Id& graphId, const Id& objectId) override;

private:
   void saveGraph(const StrategyGraph& graph);

   IStrategy& mStrategy_v1;
   std::unique_ptr<DatabaseTable> mGraphsStorage;
};

}