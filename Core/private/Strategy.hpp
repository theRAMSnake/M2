#pragma once

#include "../IStrategy.hpp"
#include "Database.hpp"
#include "Goal.hpp"
#include "Objective.hpp"

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

   Id addResource(const Resource& res) override;
   void modifyResource(const Resource& res) override;
   void deleteResource(const Id& id) override;
   std::vector<Resource> getResources() override;

private:
   template<class T>
   void loadCollection(DatabaseTable& storage, std::map<Id, std::shared_ptr<T>>& out)
   {
      storage.foreach([&](std::string id, std::string json) 
      {
         std::shared_ptr<T> item(new T(json));
         out.insert({id, item});
         item->OnChanged.connect(std::bind(&Strategy::saveItem<T>, this, std::placeholders::_1));
      });
   }

   void connectResourcesWithObjectives();
   void connectObjectivesWithGoals();

   template<class T>
   void saveItem(const T& item)
   {
      getStorage<T>().store(item.getId(), item.toJson());
   }

   template<class T>
   DatabaseTable& getStorage()
   {
      //Should never be called
      throw -1;
   }
   
   std::unique_ptr<DatabaseTable> mGoalsStorage;
   std::unique_ptr<DatabaseTable> mTasksStorage;
   std::unique_ptr<DatabaseTable> mObjectivesStorage;
   std::unique_ptr<DatabaseTable> mResourcesStorage;

   std::map<Id, std::shared_ptr<strategy::Goal>> mGoals;
   std::map<Id, Task> mTasks;
   std::map<Id, std::shared_ptr<strategy::Objective>> mObjectives;
   std::map<Id, std::shared_ptr<strategy::Resource>> mResources; 
};

}