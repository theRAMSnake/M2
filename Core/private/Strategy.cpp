#include "Strategy.hpp"
#include "Logger.hpp"
#include <boost/signals2/signal.hpp>
#include "JsonSerializer.hpp"

BIND_JSON5(materia::Task, id, parentGoalId, name, notes, done)

namespace materia
{

Strategy::Strategy(Database& db)
: mGoalsStorage(db.getTable("goals"))
, mTasksStorage(db.getTable("tasks"))
, mObjectivesStorage(db.getTable("objectives"))
, mResourcesStorage(db.getTable("resources"))
{
   loadCollection(*mGoalsStorage, mGoals);
   loadCollection(*mObjectivesStorage, mObjectives);
   loadCollection(*mResourcesStorage, mResources);

   mTasksStorage->foreach([&](std::string id, std::string json) 
   {
      mTasks.insert({id, readJson<materia::Task>(json)});
   });

   connectResourcesWithObjectives();
   connectObjectivesWithGoals();
}

Id Strategy::addGoal(const Goal& goal)
{
    auto g = goal;
    g.id = Id::generate();
    g.achieved = false;

    std::shared_ptr<strategy::Goal> newGoal(new strategy::Goal(g));

    mGoals.insert(std::make_pair(g.id, newGoal));
    newGoal->OnChanged.connect(std::bind(&Strategy::saveItem<strategy::Goal>, this, std::placeholders::_1));

    saveItem(*newGoal);

    return g.id;
}

void Strategy::modifyGoal(const Goal& goal)
{
    auto pos = mGoals.find(goal.id);
    if(pos != mGoals.end())
    {
        pos->second->accept(goal);
    }
}

void Strategy::deleteGoal(const Id& id)
{
   auto pos = mGoals.find(id);
   if(pos != mGoals.end())
   {
       auto taskIter = std::find_if(mTasks.begin(), mTasks.end(), [=](auto t)->bool {return id == t.second.parentGoalId;});
       while(taskIter != mTasks.end())
       { 
          deleteTask(taskIter->first);
          taskIter = std::find_if(mTasks.begin(), mTasks.end(), [=](auto t)->bool {return id == t.second.parentGoalId;});
       }

       for(auto o : pos->second->getObjectives())
       {
          deleteObjective(o);
       }

       mGoals.erase(pos);
       mGoalsStorage->erase(id);
   }
}

std::vector<Goal> Strategy::getGoals()
{
    std::vector<Goal> result;

    for(auto x : mGoals)
    {
        result.push_back(x.second->getProps());
    }

    return result;
}

std::optional<Goal> Strategy::getGoal(const Id& id)
{
   auto pos = mGoals.find(id);
   if(pos != mGoals.end())
   {
       return pos->second->getProps();
   }

   return std::optional<Goal>();
}

std::tuple<std::vector<Task>, std::vector<Objective>> Strategy::getGoalItems(const Id& id)
{
    std::vector<Task> tasks;
    std::vector<Objective> objs;

    for(auto x : mTasks)
    {
        if(x.second.parentGoalId == id)
        {
            tasks.push_back(x.second);
        }
    }

    for(auto x : mObjectives)
    {
        auto props = x.second->getProps();
        if(props.parentGoalId == id)
        {
            objs.push_back(props);
        }
    }

    LOG("Fetched tasks for goal: " + id.getGuid() + ", " + std::to_string(tasks.size()));

    return {tasks, objs};
}

Id Strategy::addObjective(const Objective& obj)
{
    Objective o = obj;
    o.id = Id::generate();
    
    auto parentPos = mGoals.find(o.parentGoalId);

    if(parentPos != mGoals.end())
    {
        std::shared_ptr<strategy::Objective> newObjective(new strategy::Objective(o));
        newObjective->OnChanged.connect(std::bind(&Strategy::saveItem<strategy::Objective>, this, std::placeholders::_1));

        if(o.resourceId != Id::Invalid)
        {
            auto resPos = mResources.find(o.resourceId);
            if(resPos != mResources.end())
            {
                newObjective->connect(*resPos->second);
            }
            else
            {
                //LOG;
            }
        }

        parentPos->second->connect(newObjective);
        mObjectives.insert(std::make_pair(o.id, newObjective));
        saveItem(*newObjective);

        return o.id;
    }

    return Id::Invalid;
}

void Strategy::modifyObjective(const Objective& obj)
{
    auto oldObj = mObjectives.find(obj.id);

    if(oldObj != mObjectives.end())
    {
        auto oldParent = mGoals.find(oldObj->second->getProps().parentGoalId);
        auto newParent = mGoals.find(obj.parentGoalId);

        if(newParent != mGoals.end())
        {
            if(obj.resourceId != Id::Invalid)
            {
                auto res = mResources.find(obj.resourceId);
                if(res == mResources.end())
                {
                    //LOG;
                }
                else
                {
                    oldObj->second->connect(*res->second);
                }
            }

            oldObj->second->accept(obj);

            if(newParent != oldParent)
            {
                oldParent->second->disconnect(oldObj->second);
                newParent->second->connect(oldObj->second);
            }
        }
    }
}

void Strategy::deleteObjective(const Id& id)
{
    auto oPos = mObjectives.find(id);

    if(oPos != mObjectives.end())
    {
        auto parentId = oPos->second->getProps().parentGoalId;
        auto parent = mGoals.find(parentId);

        if(parent != mGoals.end())
        {
            parent->second->disconnect(oPos->second);

            mObjectives.erase(oPos);
            mObjectivesStorage->erase(id);
        }
    }
}

Id Strategy::addTask(const Task& task)
{
    Task t = task;
    t.id = Id::generate();
    
    mTasks.insert({t.id, t});
    mTasksStorage->store(t.id, writeJson(t));
    return t.id;
}

void Strategy::modifyTask(const Task& task)
{
   auto pos = mTasks.find(task.id);
   if(pos != mTasks.end())
   {
       mTasks.erase(pos);
       mTasks.insert({task.id, task});

       mTasksStorage->store(task.id, writeJson(task));
   }
}

void Strategy::deleteTask(const Id& id)
{
   auto pos = mTasks.find(id);
   if(pos != mTasks.end())
   {
       mTasks.erase(pos);
       mTasksStorage->erase(id);
   }
}

Id Strategy::addResource(const Resource& res)
{
    auto m = res;
    m.id = Id::generate();

    std::shared_ptr<strategy::Resource> newMeas(new strategy::Resource(m));

    mResources.insert(std::make_pair(m.id, newMeas));
    newMeas->OnChanged.connect(std::bind(&Strategy::saveItem<strategy::Resource>, this, std::placeholders::_1));
    saveItem(*newMeas);

    return m.id;
}

void Strategy::modifyResource(const Resource& res)
{
    auto pos = mResources.find(res.id);
    if(pos != mResources.end())
    {
        pos->second->accept(res);
    }
}

void Strategy::deleteResource(const Id& id)
{
    auto res = mResources.find(id);
    if(res != mResources.end())
    {
        for(auto o : mObjectives)
        {
            if(o.second->getProps().resourceId == id)
            {
                o.second->disconnect(*res->second);
            }
        }

        mResources.erase(id);
        mResourcesStorage->erase(id);
    }
}

std::vector<Resource> Strategy::getResources()
{
    std::vector<Resource> result;

    for(auto x : mResources)
    {
        result.push_back(x.second->getProps());
    }

    return result;
}

void Strategy::connectResourcesWithObjectives()
{
    for(auto o : mObjectives)
    {
        auto resId = o.second->getProps().resourceId;
        if(resId != Id::Invalid)
        {
            auto resource = mResources.find(resId);
            if(resource != mResources.end())
            {
                o.second->connect(*resource->second);
            }
        }
    }
}

void Strategy::connectObjectivesWithGoals()
{
    for(auto o : mObjectives)
    {
        auto& objective = *o.second;
        auto parentGoal = mGoals.find(objective.getProps().parentGoalId);
        if(parentGoal != mGoals.end())
        {
            parentGoal->second->connect(o.second);
        }
    }
}

bool Goal::operator == (const Goal& other) const
{
   return id == other.id
      && name == other.name
      && notes == other.notes
      && achieved == other.achieved
      && focused == other.focused;
}

bool Goal::operator != (const Goal& other) const
{
   return !operator==(other);
}

bool Task::operator == (const Task& other) const
{
   return id == other.id
      && parentGoalId == other.parentGoalId
      && name == other.name
      && notes == other.notes
      && done == other.done;
}

bool Task::operator != (const Task& other) const
{
   return !operator==(other);
}

bool Objective::operator == (const Objective& other) const
{
   return id == other.id
      && parentGoalId == other.parentGoalId
      && name == other.name
      && notes == other.notes
      && reached == other.reached
      && resourceId == other.resourceId
      && expectedResourceValue == other.expectedResourceValue;
}
bool Objective::operator != (const Objective& other) const
{
   return !operator==(other);
}

bool Resource::operator == (const Resource& other) const
{
   return id == other.id
      && name == other.name
      && value == other.value;
}

bool Resource::operator != (const Resource& other) const
{
   return !operator==(other);
}

template<>
DatabaseTable& Strategy::getStorage<strategy::Goal>()
{
    return *mGoalsStorage;
}

template<>
DatabaseTable& Strategy::getStorage<strategy::Objective>()
{
    return *mObjectivesStorage;
}

template<>
DatabaseTable& Strategy::getStorage<strategy::Resource>()
{
    return *mResourcesStorage;
}

}