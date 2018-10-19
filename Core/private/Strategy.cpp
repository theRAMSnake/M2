#include "Strategy.hpp"

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

template<>
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
   result.iconId = pt.get<std::string> ("icon_id");
   result.done = pt.get<bool> ("done");

   auto tasks = as_vector<std::string>(pt, "required_tasks");

   result.requiredTasks.resize(tasks.size());
   std::copy(tasks.begin(), tasks.end(), result.requiredTasks.begin());

   return result;
}

template<>
std::string toJson(const Task& t)
{
   boost::property_tree::ptree pt;

   pt.put ("id", t.id.getGuid());
   pt.put ("parent_goal_id", t.parentGoalId.getGuid());
   pt.put ("name", t.name);
   pt.put ("notes", t.notes);
   pt.put ("icon_id", t.iconId.getGuid());
   pt.put ("done", t.done);

   std::vector<std::string> tasksToPut(t.requiredTasks.size());
   std::transform(t.requiredTasks.begin(), t.requiredTasks.end(), tasksToPut.begin(), [](auto x)->auto {return x.getGuid();});

   put(pt, "required_tasks", tasksToPut);

   std::ostringstream buf; 
   write_json (buf, pt, false);
   return buf.str();
}

template<class TObject, class TConnection>
class ConnectedObject
{
public:
   ConnectedObject(const TObject& obj, TConnection con)
   : mObj(obj)
   , mCon(con)
   {

   }

   const TObject& get()
   {
      return mObj;
   }

   ~ConnectedObject()
   {
      mCon.disconnect();
   }

private:
   const TObject mObj;
   TConnection mCon;
};

namespace impl
{

class Measurement
{
public:
   typedef int TValue;

   boost::signals2::signal<void (const TValue& val)> OnValueChanged;

   Measurement(const materia::Measurement& props, ContainerSlot&& slot)
   : mSlot(std::move(slot))
   {
      accept(props);
   }

   Measurement(const std::string& json, ContainerSlot&& slot)
   : mSlot(std::move(slot))
   {
      boost::property_tree::ptree pt;
      std::istringstream is (json);
      read_json (is, pt);
      
      mImpl.id = pt.get<std::string> ("id");
      mImpl.name = pt.get<std::string> ("name");
      mImpl.value = pt.get<TValue> ("value");
      mImpl.iconId = pt.get<std::string> ("icon_id");
   }

   void accept(const materia::Measurement& props)
   {
      if(mImpl.value != props.value)
      {
         OnValueChanged(props.value);
      }
      
      mImpl = props;
      mSlot.put(toJson());
   }

   const materia::Measurement& getProps() const
   {
      return mImpl;
   }

private:
   std::string toJson() const
   {
      boost::property_tree::ptree pt;

      pt.put ("id", mImpl.id.getGuid());
      pt.put ("name", mImpl.name);
      pt.put ("value", mImpl.value);
      pt.put ("icon_id", mImpl.iconId.getGuid());

      std::ostringstream buf; 
      write_json (buf, pt, false);
      return buf.str();
   }

   ContainerSlot mSlot;
   materia::Measurement mImpl;
};

class Objective
{
public:
   boost::signals2::signal<void (const bool val)> OnReachedChanged;

   Objective(const materia::Objective& props, ContainerSlot&& slot)
   : mSlot(std::move(slot))
   {
      accept(props);
   }

   Objective(const std::string& json, ContainerSlot&& slot)
   : mSlot(std::move(slot))
   {
      boost::property_tree::ptree pt;
      std::istringstream is (json);
      read_json (is, pt);
      
      mImpl.id = pt.get<std::string> ("id");
      mImpl.parentGoalId = pt.get<std::string> ("parent_goal_id");
      mImpl.name = pt.get<std::string> ("name");
      mImpl.notes = pt.get<std::string> ("notes");
      mImpl.iconId = pt.get<std::string> ("icon_id");
      mImpl.measurementId = pt.get<std::string> ("meas_id");
      mImpl.expected = pt.get<Measurement::TValue> ("expected");
      mImpl.reached = pt.get<bool> ("reached");
   }

   void accept(const materia::Objective& props)
   {
      bool oldReached = mImpl.reached;
      mImpl = props;

      updateReached(oldReached);

      mSlot.put(toJson());
   }

   const materia::Objective& getProps() const
   {
      return mImpl;
   }

   void connect(Measurement& meas)
   {
      mMeasConnection.disconnect();
      mMeasConnection = meas.OnValueChanged.connect(std::bind(&Objective::OnMeasValueChanged, this, std::placeholders::_1));
      mLastKnowMeasValue = meas.getProps().value;
      /*if(updateReached(mImpl.reached))
      {
         mSlot.put(toJson());
      }*/
   }

   void disconnect(const Measurement& meas)
   {
      mImpl.measurementId = Id::Invalid;
      mMeasConnection.disconnect();
      mSlot.put(toJson());
   }

   ~Objective()
   {
      mMeasConnection.disconnect();
   }

private:
   std::string toJson() const
   {
      boost::property_tree::ptree pt;

      pt.put ("id", mImpl.id.getGuid());
      pt.put ("parent_goal_id", mImpl.parentGoalId.getGuid());
      pt.put ("name", mImpl.name);
      pt.put ("notes", mImpl.notes);
      pt.put ("icon_id", mImpl.iconId.getGuid());
      pt.put ("meas_id", mImpl.measurementId.getGuid());
      pt.put ("expected", mImpl.expected);
      pt.put ("reached", mImpl.reached);

      std::ostringstream buf; 
      write_json (buf, pt, false);
      return buf.str();
   }

   bool updateReached(const bool oldReached)
   {
      if(mImpl.measurementId != Id::Invalid)
      {
         mImpl.reached = mLastKnowMeasValue >= mImpl.expected;
      }
      
      if(mImpl.reached != oldReached)
      {
         logger << "Sending update to the parent goal\n";
         OnReachedChanged(mImpl.reached);
      }

      return mImpl.reached != oldReached;
   }

   void OnMeasValueChanged(const Measurement::TValue value)
   {
      mLastKnowMeasValue = value;
      if(updateReached(mImpl.reached))
      {
         mSlot.put(toJson());
      }
   }

   boost::signals2::connection mMeasConnection;
   Measurement::TValue mLastKnowMeasValue = 0;
   ContainerSlot mSlot;
   materia::Objective mImpl;
};

class Goal
{
public:
   boost::signals2::signal<void ()> OnAchievedChanged;

   Goal(const materia::Goal& props, ContainerSlot&& slot)
   : mSlot(std::move(slot))
   {
      mImpl.achieved = false;
      accept(props);
   }

   Goal(const std::string& json, ContainerSlot&& slot)
   : mSlot(std::move(slot))
   {
      boost::property_tree::ptree pt;
      std::istringstream is (json);
      read_json (is, pt);
      
      mImpl.id = pt.get<std::string> ("id");
      mImpl.name = pt.get<std::string> ("name");
      mImpl.notes = pt.get<std::string> ("notes");
      mImpl.iconId = pt.get<std::string> ("icon_id");
      mImpl.affinityId = pt.get<std::string> ("affinity_id");
      mImpl.focused = pt.get<bool> ("focused");
      mImpl.achieved = pt.get<bool> ("achieved");
   }

   void accept(const materia::Goal& props)
   {
      mImpl = props;

      updateAchieved();

      mSlot.put(toJson());
   }

   const materia::Goal& getProps() const
   {
      return mImpl;
   }

   void connect(const std::shared_ptr<Objective>& obj)
   {
      auto con = new ConnectedObject<std::shared_ptr<Objective>, boost::signals2::connection>(
         obj,
         obj->OnReachedChanged.connect(std::bind(&Goal::OnObjReachedChanged, this))
         );

      mObjectives.insert(std::make_pair(obj->getProps().id, con));
      
      UpdateAndSaveAchieved();
   }

   void OnObjReachedChanged()
   {
      logger << "Received signal from objective \n";
      UpdateAndSaveAchieved();
   }

   void disconnect(const std::shared_ptr<Objective>& obj)
   {
      mObjectives.erase(obj->getProps().id);

      UpdateAndSaveAchieved();
   }

   std::vector<Id> getObjectives()
   {
      decltype(getObjectives()) result;

      for(auto x : mObjectives)
      {
         result.push_back(x.second->get()->getProps().id);
      }

      return result;  
   }

private:
   std::string toJson() const
   {
      boost::property_tree::ptree pt;

      pt.put ("id", mImpl.id.getGuid());
      pt.put ("name", mImpl.name);
      pt.put ("notes", mImpl.notes);
      pt.put ("icon_id", mImpl.iconId.getGuid());
      pt.put ("affinity_id", mImpl.affinityId.getGuid());
      pt.put ("focused", mImpl.focused);
      pt.put ("achieved", mImpl.achieved);

      std::ostringstream buf; 
      write_json (buf, pt, false);
      return buf.str();
   }

   void UpdateAndSaveAchieved()
   {
      if(updateAchieved())
      {
         mSlot.put(toJson());
      }
   }

   bool updateAchieved()
   {
      bool newAchieved = calculateAchieved();
      if(mImpl.achieved != newAchieved)
      {
         mImpl.achieved = newAchieved;
         OnAchievedChanged();
         return true;
      }

      return false;
   }

   bool calculateAchieved()
   {
      logger << "Calculating achieved for obs count: " << mObjectives.size() << "\n";

      bool result = false;

      if(!mObjectives.empty())
      {
         result = true;
         for(auto o : mObjectives)
         {
            logger << "obj->reached: " << o.second->get()->getProps().reached << "\n";
            result = result && o.second->get()->getProps().reached;
            if(!result)
            {
               return false;
            }
         }
      }

      return result;
   }

   std::map<Id, std::shared_ptr<ConnectedObject<std::shared_ptr<Objective>, boost::signals2::connection>>> mObjectives;
   ContainerSlot mSlot;
   materia::Goal mImpl;
};

}

Strategy::Strategy(Database& db)
: mDb(db)
{
   loadItems(); //Don't forget to connect

   connectMeasurementsWithObjectives();
   connectObjectivesWithGoals();
}

Id Strategy::addGoal(const Goal& goal)
{
    auto g = goal;
    g.id = Id::generate();
    g.achieved = false;

    std::shared_ptr<impl::Goal> newGoal(new impl::Goal(g));

    mGoals.insert(std::make_pair(g.id, newGoal));
    newGoal->OnChanged.connect(std::bind(&Strategy::saveItem, this, *newGoal));

    return g.id;
}

void Strategy::modifyGoal(const Goal& goal)
{
    auto pos = mGoals.find(goal.id);
    if(pos != mGoals.end())
    {
        auto g = *pos->second;

        g.accept(goal);
    }
}

void Strategy::deleteGoal(const Id& id)
{
   auto pos = mGoals.find(id);
   if(pos != mGoals.end())
   {
       auto taskIter = std::find_if(mTasks.begin(), mTasks.end(), [=](auto t)->bool {return id == t.parentGoalId;});
       while(taskIter != mTasks.end())
       { 
          mTasks.erase(taskIter);
          taskIter = std::find_if(mTasks.begin(), mTasks.end(), [=](auto t)->bool {return id == t.parentGoalId;});
       }

       for(auto o : g->getObjectives())
       {
          mObjectives.erase(o);
       }

       mGoals.erase(pos);
       mDb.erase(id);
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
       return pos->second.getProps();
   }

   return std::optional<Goal>();
}

std::tuple<std::vector<Task>, std::vector<Objective>> Strategy::getGoalItems(const Id& id)
{
    std::vector<Task> tasks;
    std::vector<Objective> objs;

    for(auto x : mTasks)
    {
        if(x.parentGoalId == id)
        {
            tasks.push_back(x);
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
        std::shared_ptr<impl::Objective> newObjective(new impl::Objective(o));
        newObjective->OnChanged.connect(std::bind(&Strategy::saveItem, this, *newObjective));

        if(o.measurementId != Id::Invalid)
        {
            auto measPos = mMeasurements.find(o.measurementId);
            if(measPos != mMeasurements.end())
            {
                newObjective->connect(*meas->second);
            }
            else
            {
                LOG;
            }
        }

        parentPos->second->connect(newObjective);
        mObjectives.insert(std::make_pair(o.id, newObjective));

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
        auto newParent = mGoals.find(newObj.parentGoalId);

        if(newParent != mGoals.end())
        {
            if(obj.measurementId != Id::Invalid)
            {
                auto meas = mMeasurements.find(newObj.measurementId);
                if(meas == mMeasurements.end())
                {
                    LOG;
                }
                else
                {
                    oldObj->second->connect(*meas->second);
                }
            }

            oldObj->second->accept(newObj);

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
            mDb.erase(id);
        }
    }
}

Id Strategy::addTask(const Task& task)
{
    Task t = task;
    t.id = Id::generate();
    
    mTasks.insert(t);
    mDb.insert(t);
    return t.id;
}

void Strategy::modifyTask(const Task& task)
{
   auto pos = mTask.find(item.id);
   if(pos != mTasks.end())
   {
       *pos = task;

       mDb.store(task.id, toJson(task));
   }
}

void Strategy::deleteTask(const Id& id)
{
   auto pos = mTask.find(id);
   if(pos != mTasks.end())
   {
       mTasks.erase(pos);
       mDb.erase(id);
   }
}

Id Strategy::addMeasurement(const Measurement& meas)
{
    auto m = meas;
    m.id = Id::generate();

    std::shared_ptr<impl::Measurement> newMeas(new impl::Measurement(m));

    mMeasurements.insert(std::make_pair(g.id, newGoal));
    newMeas->OnChanged.connect(std::bind(&Strategy::saveItem, this, *newMeas));

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
        mDb.erase(id);
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
      && affinityId == other.affinityId
      && achieved == other.achieved
      && iconId == other.iconId
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
      && requiredTasks == other.requiredTasks
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
      && expected == other.expected;
}
bool Objective::operator != (const Objective& other) const
{
   return !operator==(other);
}

bool Measurement::operator == (const Measurement& other) const
{
   return id == other.id
      && iconId == other.iconId
      && name == other.name
      && value == other.value;
}

bool Measurement::operator != (const Measurement& other) const
{
   return !operator==(other);
}

bool Affinity::operator == (const Affinity& other) const
{
   return id == other.id
      && iconId == other.iconId
      && name == other.name
      && colorName == other.colorName;
}

bool Affinity::operator != (const Affinity& other) const
{
   return !operator==(other);
}

}