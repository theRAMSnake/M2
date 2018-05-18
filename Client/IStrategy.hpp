#pragma once

#include "Id.hpp"
#include "IContainer.hpp"

namespace materia
{

struct StrategyItem
{
   Id id;
   std::string name;
   std::string notes;
   Id iconId;
};

struct Goal : public StrategyItem
{
   bool focused;
   bool achieved;
   Id affinityId;

   bool operator == (const Goal& other) const;
   bool operator != (const Goal& other) const;
};

struct Objective : public StrategyItem
{
   bool reached;
   Id measurementId;
   Id parentGoalId;
   int expected;

   bool operator == (const Objective& other) const;
   bool operator != (const Objective& other) const;
};

struct Task : public StrategyItem
{
   bool done;
   Id parentGoalId;
   std::vector<Id> requiredTasks;

   bool operator == (const Task& other) const;
   bool operator != (const Task& other) const;
};

struct Measurement
{
   Id id;
   std::string name;
   Id iconId;
   int value;

   bool operator == (const Measurement& other) const;
   bool operator != (const Measurement& other) const;
};

struct Affinity
{
   Id id;
   std::string name;
   Id iconId;
   std::string colorName;

   bool operator == (const Affinity& other) const;
   bool operator != (const Affinity& other) const;
};

class IStrategy
{
public:

   virtual Id addGoal(const Goal& goal) = 0;
   virtual bool modifyGoal(const Goal& goal) = 0;
   virtual bool deleteGoal(const Id& id) = 0;
   virtual std::vector<Goal> getGoals() = 0;
   virtual std::optional<Goal> getGoal(const Id& id) = 0;
   virtual std::tuple<std::vector<Task>, std::vector<Objective>> getGoalItems(const Id& id) = 0;

   virtual Id addObjective(const Objective& obj) = 0;
   virtual bool modifyObjective(const Objective& obj) = 0;
   virtual bool deleteObjective(const Id& id) = 0;

   virtual Id addTask(const Task& task) = 0;
   virtual bool modifyTask(const Task& task) = 0;
   virtual bool deleteTask(const Id& id) = 0;

   virtual Id addMeasurement(const Measurement& meas) = 0;
   virtual bool modifyMeasurement(const Measurement& meas) = 0;
   virtual bool deleteMeasurement(const Id& id) = 0;
   virtual std::vector<Measurement> getMeasurements() = 0;

   virtual void configureAffinities(const std::vector<Affinity>& affinites) = 0;
   virtual std::vector<Affinity> getAffinities() = 0;

   virtual void clear() = 0;

   virtual ~IStrategy(){}
};

}