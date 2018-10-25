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
      materia::Id parentGoalId;
      std::string title;
      std::string notes;
   };

   struct Goal
   {
      materia::Id id;
      std::string title;
      std::string notes;
      bool focused;
      bool achieved;
   };

   void modifyTask(const Task& task);
   void deleteTask(const materia::Id& id);
   std::vector<Task> getActiveTasks();

private:
   void fetchGoals();

   std::vector<Goal> mGoals;
   MateriaServiceProxy<strategy::StrategyService> mService;
};