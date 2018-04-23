#pragma once

#include "MateriaServiceProxy.hpp"
#include "Id.hpp"
#include "Container.hpp"

#include "messages/strategy.pb.h"

namespace materia
{

struct StrategyItem
{
   Id id;
   Id parentGoalId;
   
   std::string name;
   std::string notes;
   Id iconId;
};

struct Goal : public StrategyItem
{
   bool focused;
   bool achieved;
   std::vector<Id> requiredGoals;
   Id affinityId;

   bool operator == (const Goal& other) const;
};

struct Objective : public StrategyItem
{
   bool reached;
   Id measurementId;
};

struct Task : public StrategyItem
{
   bool done;
   Id actionReference;
   Id calendarReference;
   int count;
   std::vector<Id> requiredTasks;
};

struct Measurement
{
   Id id;
   std::string name;
   Id iconId;
   int value;
};

struct Affinity
{
   Id id;
   std::string name;
   Id iconId;
   std::string colorName;
};

class Strategy
{
public:
   Strategy(materia::ZmqPbChannel& channel);

   Id addGoal(const Goal& goal);
   bool modifyGoal(const Goal& goal);
   bool deleteGoal(const Id& id);
   std::vector<Goal> getGoals();
   std::tuple<std::vector<Task>, std::vector<Objective>> getGoalItems(const Id& id);

   Id addObjective(const Objective& obj);
   bool modifyObjective(const Objective& obj);
   bool deleteObjective(const Id& id);

   Id addTask(const Task& task);
   bool modifyTask(const Task& task);
   bool deleteTask(const Id& id);

   Id addMeasurement(const Measurement& meas);
   bool modifyMeasurement(const Measurement& meas);
   bool deleteMeasurement(const Id& id);
   std::vector<Measurement> getMeasurements();

   void configureAffinities(const std::vector<Affinity>& affinites);
   std::vector<Affinity> getAffinities();

private:
   MateriaServiceProxy<strategy::StrategyService> mProxy;
};

}