#pragma once

#include <Common/Id.hpp>
#include <vector>
#include <map>
#include <memory>
#include "NodeAttributes.hpp"

namespace materia
{

struct Goal
{
   Id id;
   std::string name;
   std::string notes;
   bool focused;
   bool achieved;
};

enum class NodeType
{
   //keep consistency
   Blank = 0,

   //Auto created with graph
   //Cannot be created
   //Cannot be deleted
   //Cannot be linked from
   //Completed if enabled
   Goal = 1,

   Task = 2, 
   Counter = 3,
   Watch = 4,
   Wait = 5,
   Reference = 6,
   Milestone = 7
   //Condition = 7, //condition
};

struct Node
{
   Id id;
   NodeType type;
};

struct Link
{
   Id from;
   Id to;
};

class IStrategyGraph
{
public:
   virtual const std::vector<Link>& getLinks() const = 0;
   virtual const std::vector<Node>& getNodes() const = 0;

   virtual std::optional<Node> getNode(const Id& nodeId) const = 0;

   virtual NodeAttributes getNodeAttributes(const Id& nodeId) const = 0;

   virtual ~IStrategyGraph(){}
};

using TNodeAttrs = std::map<NodeAttributeType, std::string>;

struct WatchItem
{
    Id id;
    std::string text;
};

struct FocusItem
{
   Id graphId;
   Id nodeId;
   Id id;
};

class IStrategy_v2
{
public:
   virtual Id addGoal(const Goal& goal) = 0;
   virtual void modifyGoal(const Goal& goal) = 0;
   virtual std::vector<Goal> getGoals() = 0;
   virtual std::optional<Goal> getGoal(const Id& id) = 0;
   virtual void deleteGoal(const Id& id) = 0;

   virtual std::shared_ptr<IStrategyGraph> getGraph(const Id& graphId) = 0; 

   virtual void createLink(const Id& graphId, const Id& from, const Id& to) = 0;
   virtual void breakLink(const Id& graphId, const Id& from, const Id& to) = 0;
   virtual Id createNode(const Id& graphId) = 0;
   virtual void setNodeAttributes(const Id& graphId, const Id& objectId, const NodeType& type, const NodeAttributes& attrs) = 0;
   virtual void deleteNode(const Id& graphId, const Id& objectId) = 0;

   virtual std::vector<WatchItem> getWatchItems() const = 0;
   virtual void removeWatchItem(const Id& id) = 0;
   virtual Id addWatchItem(const WatchItem& item) = 0;
   virtual void replaceWatchItem(const WatchItem& item) = 0;

   virtual std::vector<FocusItem> getFocusItems() const = 0;
   virtual void removeFocusItem(const Id& id) = 0;
   virtual void completeFocusItem(const FocusItem& item) = 0;
   virtual Id addFocusItem(const FocusItem& item) = 0;

   virtual ~IStrategy_v2(){}
};

}