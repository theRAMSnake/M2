#pragma once

#include "MateriaServiceProxy.hpp"
#include "../IStrategy.hpp"
#include "Container.hpp"

#include "messages/strategy.pb.h"

namespace materia
{

class Strategy : public IStrategy
{
public:
   Strategy(materia::ZmqPbChannel& channel);

   Id addGoal(const Goal& goal) override;
   bool modifyGoal(const Goal& goal) override;
   bool deleteGoal(const Id& id) override;
   std::vector<Goal> getGoals() override;
   std::optional<Goal> getGoal(const Id& id) override;
   std::tuple<std::vector<Task>, std::vector<Objective>> getGoalItems(const Id& id) override;

   Id addObjective(const Objective& obj) override;
   bool modifyObjective(const Objective& obj) override;
   bool deleteObjective(const Id& id) override;

   Id addTask(const Task& task) override;
   bool modifyTask(const Task& task) override;
   bool deleteTask(const Id& id) override;

   Id addMeasurement(const Measurement& meas) override;
   bool modifyMeasurement(const Measurement& meas) override;
   bool deleteMeasurement(const Id& id) override;
   std::vector<Measurement> getMeasurements() override;

   void configureAffinities(const std::vector<Affinity>& affinites) override;
   std::vector<Affinity> getAffinities() override;

   void clear() override;

private:
   MateriaServiceProxy<strategy::StrategyService> mProxy;
};

}