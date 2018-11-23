#pragma once

#include <Common/Id.hpp>
#include <vector>
#include <optional>

namespace materia
{

struct StrategyItem
{
   Id id;
   std::string name;
   std::string notes;
};

struct Goal : public StrategyItem
{
   bool focused;
   bool achieved;

   bool operator == (const Goal& other) const;
   bool operator != (const Goal& other) const;
};

struct Objective : public StrategyItem
{
   bool reached;
   Id resourceId;
   Id parentGoalId;
   int expectedResourceValue;

   bool operator == (const Objective& other) const;
   bool operator != (const Objective& other) const;
};

struct Task : public StrategyItem
{
   bool done;
   Id parentGoalId;

   bool operator == (const Task& other) const;
   bool operator != (const Task& other) const;
};

struct Resource
{
   Id id;
   std::string name;
   int value;

   bool operator == (const Resource& other) const;
   bool operator != (const Resource& other) const;
};

class IStrategy
{
public:

   virtual Id addGoal(const Goal& goal) = 0;
   virtual void modifyGoal(const Goal& goal) = 0;
   virtual void deleteGoal(const Id& id) = 0;
   virtual std::vector<Goal> getGoals() = 0;
   virtual std::optional<Goal> getGoal(const Id& id) = 0;
   virtual std::tuple<std::vector<Task>, std::vector<Objective>> getGoalItems(const Id& id) = 0;

   virtual Id addObjective(const Objective& obj) = 0;
   virtual void modifyObjective(const Objective& obj) = 0;
   virtual void deleteObjective(const Id& id) = 0;

   virtual Id addTask(const Task& task) = 0;
   virtual void modifyTask(const Task& task) = 0;
   virtual void deleteTask(const Id& id) = 0;

   virtual Id addResource(const Resource& res) = 0;
   virtual void modifyResource(const Resource& res) = 0;
   virtual void deleteResource(const Id& id) = 0;
   virtual std::vector<Resource> getResources() = 0;

   virtual ~IStrategy(){}
};

}