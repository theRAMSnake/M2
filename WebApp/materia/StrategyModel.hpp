#pragma once

#include <vector>
#include <Common/Id.hpp>
#include "ZmqPbChannel.hpp"
#include "MateriaServiceProxy.hpp"
#include <messages/strategy.pb.h>

class StrategyModel
{
public:
   StrategyModel(ZmqPbChannel& channel);

   struct Task
   {
      materia::Id id;
      materia::Id graphId;
      materia::Id nodeId;
      std::string title;
   };

   struct Objective
   {
      materia::Id id;
      std::string title;
      bool reached;
      materia::Id parentGoalId;
      materia::Id resId;
      int expectedResValue;
   };

   struct Goal
   {
      materia::Id id;
      std::string title;
      std::string notes;
      bool focused;
      bool achieved;
   };

   struct Node
   {
      //!Refactor?
      materia::Id id;
      strategy::NodeType type;
      std::string descriptiveTitle;
      std::string brief;
      bool isDone;
      std::pair<int, int> progress;
      materia::Id watchItemReference;
      materia::Id graphReference;
      std::time_t requiredTimestamp;
   };

   struct Link
   {
      materia::Id from;
      materia::Id to;
   };

   struct Graph
   {
      std::vector<Node> nodes;
      std::vector<Link> links;
   };

   struct Resource
   {
      materia::Id id;
      std::string name;
      int value;
   };

   struct WatchItem
   {
      materia::Id id;
      std::string title;
   };

   void deleteTask(const Task& task);
   void completeTask(const Task& task);
   std::vector<Task> getActiveTasks();

   std::vector<Goal> getGoals();
   std::optional<Goal> getGoal(const materia::Id& id);
   const Goal& addGoal(const bool focused, const std::string& name);
   void modifyGoal(const Goal& goal);
   void deleteGoal(const materia::Id& id);
   std::vector<Task> getGoalTasks(const materia::Id& id);
   std::vector<Objective> getGoalObjectives(const materia::Id& id);

   std::vector<Resource> getResources();
   std::optional<Resource> getResource(const std::string& name);
   Resource addResource(const std::string& name);
   void modifyResource(const Resource& r);
   void deleteResource(const materia::Id& id);

   Objective addObjective(const std::string& title, const materia::Id& parentGoalId);
   void deleteObjective(const materia::Id& id);
   Objective modifyObjective(const Objective& o);

   std::optional<Graph> getGraph(const materia::Id& id);
   materia::Id createNode(const materia::Id& graphId);
   void deleteNode(const materia::Id& graphId, const materia::Id& nodeId);
   void updateNode(const materia::Id& graphId, const Node& node);
   void cloneNode(const materia::Id& graphId, const Node& node);
   void focusNode(const materia::Id& graphId, const Node& node);

   void createLink(const materia::Id& graphId, const materia::Id& fromNodeId, const materia::Id& toNodeId);
   void deleteLink(const materia::Id& graphId, const materia::Id& fromNodeId, const materia::Id& toNodeId);

   std::vector<WatchItem> getWatchItems();
   WatchItem addWatchItem(const std::string& name);
   void modifyWatchItem(const WatchItem& r);
   void deleteWatchItem(const materia::Id& id);

private:
   void fetchGoals();
   std::string createDescriptiveTitle(const StrategyModel::Node& node);
   std::optional<WatchItem> getWatchItem(const materia::Id& id);

   std::vector<Goal> mGoals;
   MateriaServiceProxy<strategy::StrategyService> mService;
};