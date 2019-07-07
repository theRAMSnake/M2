#pragma once

#include "../IStrategy_v2.hpp"
#include "../IStrategy.hpp"
#include "Database.hpp"
#include "StrategyGraph.hpp"

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

   std::shared_ptr<IStrategyGraph> getGraph(const Id& graphId) override;

   void createLink(const Id& graphId, const Id& nodeFrom, const Id& nodeTo) override;
   void breakLink(const Id& graphId, const Id& nodeFrom, const Id& nodeTo) override;
   Id createNode(const Id& graphId) override;
   void setNodeAttributes(const Id& graphId, const Id& objectId, const NodeType& type, const NodeAttributes& attrs) override;
   void deleteNode(const Id& graphId, const Id& objectId) override;

   std::vector<WatchItem> getWatchItems() const override;
   void removeWatchItem(const Id& id) override;
   Id addWatchItem(const WatchItem& item) override;
   void replaceWatchItem(const WatchItem& item) override;

   std::vector<FocusItem> getFocusItems() const override;
   void removeFocusItem(const Id& id) override;
   void completeFocusItem(const FocusItem& item) override;
   Id addFocusItem(const FocusItem& item) override;

private:
   std::shared_ptr<StrategyGraph> loadGraph(const Id& graphId);
   void saveGraph(const StrategyGraph& graph);
   void updateCompleteness(StrategyGraph& graph);
   void updateNodeCompleteness(const StrategyGraph& graph, const Node& node, std::map<Id, bool>& completenessPerNode);
   bool getNodeSelfCompleteness(const StrategyGraph& graph, const Node& node);

   template<class F>
   void makeGraphOperation(const Id& graphId, F f)
   {
      auto graph = loadGraph(graphId);

      if(graph)
      {
         f(*graph);
         saveGraph(*graph);
      }
   }

   IStrategy& mStrategy_v1;
   std::unique_ptr<DatabaseTable> mGraphsStorage;
   std::unique_ptr<DatabaseTable> mWatchStorage;
   std::unique_ptr<DatabaseTable> mFocusStorage;
};

}