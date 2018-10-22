#include "Strategy.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/signals2/signal.hpp>

namespace materia
{

/*
strategy::CommonItemProperties toProto(const materia::StrategyItem& x)
{
   strategy::CommonItemProperties result;

   result.mutable_id()->CopyFrom(toProto(x.id));
   

   result.set_name(x.name);
   result.set_notes(x.notes);
   result.mutable_icon_id()->CopyFrom(toProto(x.iconId));

   return result;
}

template<class T>
T fromProto(const strategy::CommonItemProperties& x)
{
   T result;

   result.id = fromProto(x.id());
   


   result.name = x.name();
   result.notes = x.notes();
   result.iconId = fromProto(x.icon_id());

   return result;
}

strategy::Goal toProto(const materia::Goal& x)
{
   strategy::Goal result;

   result.mutable_common_props()->CopyFrom(toProto(static_cast<const materia::StrategyItem&>(x)));
   result.mutable_affinityid()->CopyFrom(toProto(x.affinityId));
   result.set_focused(x.focused);
   result.set_achieved(x.achieved);

   return result;
}

materia::Goal fromProto(const strategy::Goal& x)
{
   materia::Goal result = fromProto<Goal>(x.common_props());

   result.affinityId = fromProto(x.affinityid());
   result.focused = x.focused();
   result.achieved = x.achieved();

   return result;
}

strategy::Task toProto(const materia::Task& x)
{
   strategy::Task result;

   result.mutable_common_props()->CopyFrom(toProto(static_cast<const materia::StrategyItem&>(x)));
   result.set_done(x.done);
   result.mutable_parent_goal_id()->CopyFrom(toProto(x.parentGoalId));
   for(auto a : x.requiredTasks)
   {
      result.add_required_tasks()->CopyFrom(toProto(a));
   }

   return result;
}

materia::Task fromProto(const strategy::Task& x)
{
   materia::Task result = fromProto<Task>(x.common_props());

   result.done = x.done();
   result.parentGoalId = fromProto(x.parent_goal_id());
   for(auto a : x.required_tasks())
   {
      result.requiredTasks.push_back(fromProto(a));
   }

   return result;
}

strategy::Objective toProto(const materia::Objective& x)
{
   strategy::Objective result;

   result.mutable_common_props()->CopyFrom(toProto(static_cast<const materia::StrategyItem&>(x)));
   result.mutable_meas_id()->CopyFrom(toProto(x.measurementId));
   result.mutable_parent_goal_id()->CopyFrom(toProto(x.parentGoalId));
   result.set_reached(x.reached);
   result.set_expectedtreshold(x.expected);

   return result;
}

materia::Objective fromProto(const strategy::Objective& x)
{
   materia::Objective result = fromProto<Objective>(x.common_props());

   result.measurementId = fromProto(x.meas_id());
   result.parentGoalId = fromProto(x.parent_goal_id());
   result.reached = x.reached();
   result.expected = x.expectedtreshold();

   return result;
}

strategy::Measurement toProto(const materia::Measurement& x)
{
   strategy::Measurement result;

   result.mutable_id()->CopyFrom(toProto(x.id));
   result.set_value(x.value);
   result.set_name(x.name);
   result.mutable_icon_id()->CopyFrom(toProto(x.iconId));

   return result;
}

materia::Measurement fromProto(const strategy::Measurement& x)
{
   materia::Measurement result;

   result.id = fromProto(x.id());
   result.name = x.name();
   result.value = x.value();
   result.iconId = fromProto(x.icon_id());

   return result;
}

strategy::Affinity toProto(const materia::Affinity& x)
{
   strategy::Affinity result;

   result.mutable_id()->CopyFrom(toProto(x.id));
   result.set_name(x.name);
   result.set_colorname(x.colorName);
   result.mutable_icon_id()->CopyFrom(toProto(x.iconId));

   return result;
}

materia::Affinity fromProto(const strategy::Affinity& x)
{
   materia::Affinity result;

   result.id = fromProto(x.id());
   result.name = x.name();
   result.colorName = x.colorname();
   result.iconId = fromProto(x.icon_id());

   return result;
}
*/

Task fromJson(const std::string& content)
{
   Task result;

   boost::property_tree::ptree pt;
   std::istringstream is (content);
   read_json (is, pt);
   
   result.id = pt.get<std::string> ("id");
   result.parentGoalId = pt.get<std::string> ("parent_goal_id");
   result.name = pt.get<std::string> ("name");
   result.notes = pt.get<std::string> ("notes");
   result.done = pt.get<bool> ("done");

   return result;
}

std::string toJson(const Task& t)
{
   boost::property_tree::ptree pt;

   pt.put ("id", t.id.getGuid());
   pt.put ("parent_goal_id", t.parentGoalId.getGuid());
   pt.put ("name", t.name);
   pt.put ("notes", t.notes);
   pt.put ("done", t.done);

   std::ostringstream buf; 
   write_json (buf, pt, false);
   return buf.str();
}

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
      mTasks.insert({id, fromJson(json)});
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
    mTasksStorage->store(t.id, toJson(t));
    return t.id;
}

void Strategy::modifyTask(const Task& task)
{
   auto pos = mTasks.find(task.id);
   if(pos != mTasks.end())
   {
       mTasks.erase(pos);
       mTasks.insert({task.id, task});

       mTasksStorage->store(task.id, toJson(task));
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

}