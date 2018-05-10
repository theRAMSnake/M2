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
   Id affinityId;

   bool operator == (const Goal& other) const;
   bool operator != (const Goal& other) const;
};

struct Objective : public StrategyItem
{
   bool reached;
   Id measurementId;
   int expected;

   bool operator == (const Objective& other) const;
   bool operator != (const Objective& other) const;
};

struct Task : public StrategyItem
{
   bool done;
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

class Strategy
{
public:
   Strategy(materia::ZmqPbChannel& channel);

   Id addGoal(const Goal& goal);
   bool modifyGoal(const Goal& goal);
   bool deleteGoal(const Id& id);
   std::vector<Goal> getGoals();
   std::optional<Goal> getGoal(const Id& id);
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

   void clear();

private:
   MateriaServiceProxy<strategy::StrategyService> mProxy;
};

strategy::Goal toProto(const materia::Goal& x);
materia::Goal fromProto(const strategy::Goal& x);

strategy::Task toProto(const materia::Task& x);
materia::Task fromProto(const strategy::Task& x);

strategy::Objective toProto(const materia::Objective& x);
materia::Objective fromProto(const strategy::Objective& x);

strategy::Measurement toProto(const materia::Measurement& x);
materia::Measurement fromProto(const strategy::Measurement& x);

strategy::Affinity toProto(const materia::Affinity& x);
materia::Affinity fromProto(const strategy::Affinity& x);

}