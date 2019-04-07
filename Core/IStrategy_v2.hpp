#pragma once

#include <Common/Id.hpp>
#include <vector>
#include "IStrategy.hpp"

namespace materia
{

enum class NodeType
{
   Blank = 0,

   //Auto created with graph
   //Cannot be created
   //Cannot be deleted
   //Cannot be linked from
   //Completed if enabled
   Goal = 1,

   Task = 2, //simple
   Counter = 3,
   Objective = 4, //simple
   Watch = 5, //simple
   //Wait = 6, //timestamp >>> curTime >= timestamp
   //Condition = 7, //condition
   //Reference = 8 //other graph node completed
};

struct SimpleNodeAttributes
{
   bool done;
   std::string brief;
};

struct CounterNodeAttributes
{
   std::string brief;
   int current;
   int required;
};

class INode
{
public:
   virtual NodeType getType() const = 0;
   virtual bool enabled() const = 0;
   virtual bool completed() const = 0;

   virtual SimpleNodeAttributes getSimpleAttrs() const = 0;
   virtual CounterNodeAttributes getCounterAttrs() const = 0;

   ~INode(){}
};

class IStrategyGraphReceiver
{
public:

   virtual void addLink(const Id& from, const Id& to) = 0;
   virtual void addNode(const INode& node) = 0;

   ~IStrategyGraphReceiver(){}
};

class IStrategy_v2
{
public:
   virtual Id addGoal(const Goal& goal) = 0;
   virtual void modifyGoal(const Goal& goal) = 0;
   virtual std::vector<Goal> getGoals() = 0;
   virtual std::optional<Goal> getGoal(const Id& id) = 0;
   virtual void deleteGoal(const Id& id) = 0;

   virtual bool getGraph(const IStrategyGraphReceiver& receiver) = 0; 

   virtual void createLink(const Id& graphId, const Id& from, const Id& to) = 0;
   virtual void breakLink(const Id& graphId, const Id& from, const Id& to) = 0;

   virtual Id createNode(const Id& graphId) = 0;

   virtual void setNodeAttributes(const Id& graphId, const Id& objectId, const SimpleNodeAttributes& attrs) = 0;
   virtual void setNodeAttributes(const Id& graphId, const Id& objectId, const CounterNodeAttributes& attrs) = 0;

   virtual void deleteNode(const Id& graphId, const Id& objectId) = 0;

   virtual ~IStrategy_v2(){}
};

}