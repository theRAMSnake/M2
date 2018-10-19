#pragma once

#include "../IStrategy.hpp"

namespace materia
{

class Strategy : public IStrategy
{
public:
   Strategy(Database& db);

   Id addGoal(const Goal& goal) override;
   void modifyGoal(const Goal& goal) override;
   void deleteGoal(const Id& id) override;
   std::vector<Goal> getGoals() override;
   std::optional<Goal> getGoal(const Id& id) override;
   std::tuple<std::vector<Task>, std::vector<Objective>> getGoalItems(const Id& id) override;

   Id addObjective(const Objective& obj) override;
   void modifyObjective(const Objective& obj) override;
   void deleteObjective(const Id& id) override;

   Id addTask(const Task& task) override;
   void modifyTask(const Task& task) override;
   void deleteTask(const Id& id) override;

   Id addMeasurement(const Measurement& meas) override;
   void modifyMeasurement(const Measurement& meas) override;
   void deleteMeasurement(const Id& id) override;
   std::vector<Measurement> getMeasurements() override;

private:
   void connectMeasurementsWithObjectives();
   void connectObjectivesWithGoals();
   Database mDb;
};

}