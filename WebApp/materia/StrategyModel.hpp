#pragma once

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
      std::string title;
      std::string notes;
      materia::Id parentGoalId;
   };

   struct Goal
   {
      materia::Id id;
      std::string title;
      std::string notes;
      bool focused;
      bool achieved;
   };

   struct Resource
   {
      materia::Id id;
      std::string name;
      int value;
   };

   Task addTask(const std::string& title, const materia::Id& parentGoalId);
   void modifyTask(const Task& task);
   void deleteTask(const materia::Id& id);
   std::vector<Task> getActiveTasks();

   std::vector<Goal> getGoals();
   const Goal& addGoal(const bool focused, const std::string& name);
   void modifyGoal(const Goal& goal);
   void deleteGoal(const materia::Id& id);
   std::vector<Task> getGoalTasks(const materia::Id& id);

   std::vector<Resource> getResources();

private:
   void fetchGoals();

   std::vector<Goal> mGoals;
   MateriaServiceProxy<strategy::StrategyService> mService;
};