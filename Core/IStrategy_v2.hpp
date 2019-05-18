#pragma once

#include <Common/Id.hpp>
#include <vector>
#include <map>
#include <memory>
#include "IStrategy.hpp"

namespace materia
{

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
   Watch = 5,
   //Wait = 6, //timestamp >>> curTime >= timestamp
   //Condition = 7, //condition
   //Reference = 8 //other graph node completed
};

struct Node
{
   Id id;
   NodeType type;
};

enum class NodeAttributeType
{
   //keep consistency
   IS_DONE = 1,
   BRIEF = 2,
   PROGRESS_TOTAL = 3,
   PROGRESS_CURRENT = 4
};

struct Link
{
   Id from;
   Id to;
};

class IStrategyGraph
{
public:
   virtual std::vector<Link> getLinks() const = 0;
   virtual std::vector<Node> getNodes() const = 0;

   virtual std::map<NodeAttributeType, std::string> getNodeAttributes(const Id& nodeId) const = 0;

   virtual ~IStrategyGraph(){}
};

using TNodeAttrs = std::map<NodeAttributeType, std::string>;

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

   virtual void setNodeAttributes(const Id& graphId, const Id& objectId, const NodeType& type, const std::map<NodeAttributeType, std::string>& attrs) = 0;

   virtual void deleteNode(const Id& graphId, const Id& objectId) = 0;

   virtual ~IStrategy_v2(){}
};

}