#include "Strategy.hpp"
#include <boost/signals2/signal.hpp>
#include "JsonSerializer.hpp"

BIND_JSON5(materia::Task, id, parentGoalId, name, notes, done)

namespace materia
{

Strategy::Strategy(Database& db)
: mGoalsStorage(db.getTable("goals"))
, mTasksStorage(db.getTable("tasks"))
, mObjectivesStorage(db.getTable("objectives"))
, mMeasurementsStorage(db.getTable("measurements"))
{
   loadCollection(*mGoalsStorage, mGoals);
   loadCollection(*mObjectivesStorage, mObjectives);
   loadCollection(*mMeasurementsStorage, mMeasurements);

   mTasksStorage->foreach([&](std::string id, std::string json) 
   {
      mTasks.insert({id, readJson<materia::Task>(json)});
   });

   connectMeasurementsWithObjectives();
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

        if(o.measurementId != Id::Invalid)
        {
            auto measPos = mMeasurements.find(o.measurementId);
            if(measPos != mMeasurements.end())
            {
                newObjective->connect(*measPos->second);
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
            if(obj.measurementId != Id::Invalid)
            {
                auto meas = mMeasurements.find(obj.measurementId);
                if(meas == mMeasurements.end())
                {
                    //LOG;
                }
                else
                {
                    oldObj->second->connect(*meas->second);
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

Id Strategy::addMeasurement(const Measurement& meas)
{
    auto m = meas;
    m.id = Id::generate();

    std::shared_ptr<strategy::Measurement> newMeas(new strategy::Measurement(m));

    mMeasurements.insert(std::make_pair(m.id, newMeas));
    newMeas->OnChanged.connect(std::bind(&Strategy::saveItem<strategy::Measurement>, this, std::placeholders::_1));
    saveItem(*newMeas);

    return m.id;
}

void Strategy::modifyMeasurement(const Measurement& meas)
{
    auto pos = mMeasurements.find(meas.id);
    if(pos != mMeasurements.end())
    {
        pos->second->accept(meas);
    }
}

void Strategy::deleteMeasurement(const Id& id)
{
    auto meas = mMeasurements.find(id);
    if(meas != mMeasurements.end())
    {
        for(auto o : mObjectives)
        {
            if(o.second->getProps().measurementId == id)
            {
                o.second->disconnect(*meas->second);
            }
        }

        mMeasurements.erase(id);
        mMeasurementsStorage->erase(id);
    }
}

std::vector<Measurement> Strategy::getMeasurements()
{
    std::vector<Measurement> result;

    for(auto x : mMeasurements)
    {
        result.push_back(x.second->getProps());
    }

    return result;
}

void Strategy::connectMeasurementsWithObjectives()
{
    for(auto o : mObjectives)
    {
        auto measId = o.second->getProps().measurementId;
        if(measId != Id::Invalid)
        {
            auto measurement = mMeasurements.find(measId);
            if(measurement != mMeasurements.end())
            {
                o.second->connect(*measurement->second);
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
      && measurementId == other.measurementId
      && expectedMeasurementValue == other.expectedMeasurementValue;
}
bool Objective::operator != (const Objective& other) const
{
   return !operator==(other);
}

bool Measurement::operator == (const Measurement& other) const
{
   return id == other.id
      && name == other.name
      && value == other.value;
}

bool Measurement::operator != (const Measurement& other) const
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
DatabaseTable& Strategy::getStorage<strategy::Measurement>()
{
    return *mMeasurementsStorage;
}

}