#include <iostream>
#include <fstream>
#include <Common/InterprocessService.hpp>
#include <Common/PortLayout.hpp>
#include <Client/MateriaClient.hpp>
#include <messages/strategy.pb.h>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/signals2.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "RemoteCollection.hpp"

namespace materia
{

boost::uuids::random_generator generator;

/*template<>
std::string toJson(const Goal& g)
{
   boost::property_tree::ptree pt;

   pt.put ("id", g.id.getGuid());
   pt.put ("parent_id", g.parentGoalId.getGuid());
   pt.put ("name", g.name);
   pt.put ("notes", g.notes);
   pt.put ("icon_id", g.iconId.getGuid());
   pt.put ("affinity_id", g.affinityId.getGuid());
   pt.put ("achieved", g.achieved);
   pt.put ("focused", g.focused);

   std::ostringstream buf; 
   write_json (buf, pt, false);
   return buf.str();
}

Goal fromJson(const std::string& json)
{
   Goal result;

   boost::property_tree::ptree pt;
   std::istringstream is (json);
   read_json (is, pt);
   
   result.id = pt.get<std::string> ("id");
   result.parentGoalId = pt.get<std::string> ("parent_id");
   result.name = pt.get<std::string> ("name");
   result.notes = pt.get<std::string> ("notes");
   result.iconId = pt.get<std::string> ("icon_id");
   result.affinityId = pt.get<std::string> ("affinity_id");
   result.achieved = pt.get<bool> ("achieved");
   result.focused = pt.get<bool> ("focused");

   return result;
}*/

template<>
Affinity fromJson(const std::string& content)
{
   Affinity result;

   boost::property_tree::ptree pt;
   std::istringstream is (content);
   read_json (is, pt);
   
   result.id = pt.get<std::string> ("id");
   result.name = pt.get<std::string> ("name");
   result.colorName = pt.get<std::string> ("colorName");
   result.iconId = pt.get<std::string> ("icon_id");

   return result;
}

template<>
std::string toJson(const Affinity& t)
{
   boost::property_tree::ptree pt;

   pt.put ("id", t.id.getGuid());
   pt.put ("name", t.name);
   pt.put ("colorName", t.colorName);
   pt.put ("icon_id", t.iconId.getGuid());

   std::ostringstream buf; 
   write_json (buf, pt, false);
   return buf.str();
}  

namespace impl
{

template<class TObject, class TConnection>
class ConnectedObject
{
public:
   ConnectedObject(TObject& obj, TConnection& con)
   : mObj(obj)
   , mCon(con)
   {

   }

   TObject& get()
   {
      return mObj;
   }

   ~ConnectedObject()
   {
      mCon.disconnect();
   }

private:
   TObject mObj;
   TConnection mCon;
};

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
      mImpl = props;
      mSlot.put(toJson());

      if(mImpl.value != props.value)
      {
         OnValueChanged(mImpl.value);
      }
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
      mImpl.measId = pt.get<std::string> ("meas_id");
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

   void connect(const Measurement& meas)
   {
      //check measurement manipulation code to sync with objective
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
      pt.put ("meas_id", mImpl.measId.getGuid());
      pt.put ("expected", mImpl.expected);
      pt.put ("reached", mImpl.reached);

      std::ostringstream buf; 
      write_json (buf, pt, false);
      return buf.str();
   }

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
   {
      boost::property_tree::ptree pt;
      std::istringstream is (json);
      read_json (is, pt);
      
      mImpl.id = pt.get<std::string> ("id");
      mImpl.parentGoalId = pt.get<std::string> ("parent_goal_id");
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

   void registerChild(std::shared_ptr<Goal>& child)
   {
      mChildren.insert(child->getProps().id, {
         child,
         child.OnAchievedChanged.connect(std::bind(&Goal::UpdateAndSaveAchieved, this))
         });
      
      UpdateAndSaveAchieved();
   }

   void unregisterChild(std::shared_ptr<Goal>& child)
   {
      mChildren.erase(child->getProps().id);

      UpdateAndSaveAchieved();
   }

   void connect(const std::shared_ptr<Objective>& obj)
   {
      mObjectives.insert(child->getProps().id, {
         obj,
         obj.OnReachedChanged.connect(std::bind(&Goal::UpdateAndSaveAchieved, this))
         });
      
      UpdateAndSaveAchieved();
   }

   const std::vector<std::shared_ptr<Goal> getChildren() const
   {
      decltype(getChildren()) result;
      for(auto x : mChildren)
      {
         result.push_back(x->second);
      }

      return result;
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

   void updateAchieved()
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
      bool result = false;

      if(!mChildren.empty())
      {
         result = true;
         for(auto g : mChildren)
         {
            result = result && g->second->getProps().achieved;
            if(!result)
            {
               return false;
            }
         }
      }

      if(!mObjectives.empty())
      {
         for(auto o : mObjectives)
         {
            result = result && g->second->getProps().reached;
            if(!result)
            {
               return false;
            }
         }
      }

      return result;
   }

   std::map<Id, TConnectedObject<Goal, boost::signals2::connection>> mChildren;
   std::map<Id, TConnectedObject<Objective, boost::signals2::connection>> mObjectives;
   ContainerSlot mSlot;
   materia::Goal mImpl;
};

}

template<class T>
struct ItemTraits
{

};

template<>
struct ItemTraits<impl::Goal>
{
   static constexpr auto CONTAINER_NAME = "goals";
};

template<>
struct ItemTraits<impl::Measurement>
{
   static constexpr auto CONTAINER_NAME = "measurements";
};

template<>
struct ItemTraits<impl::Objective>
{
   static constexpr auto CONTAINER_NAME = "objectives";
};

class StrategyServiceImpl : public strategy::StrategyService
{
public:
   StrategyServiceImpl(Container& container, Events& events)
   : mContainer(container)
   , mEvents(events)
   , mAffinities("affinities", container)
   , mTasks("tasks", container)
   {
      //check state - might be broken if incorrect shutdown
      populateCollection(mGoals);
      populateCollection(mObjectives);
      populateCollection(mMeasurements);

      connectGoals();
      connectMeasurementsWithObjectives();
      connectObjectivesWithGoals();
   }

   void AddGoal(::google::protobuf::RpcController* controller,
      const ::strategy::Goal* request,
      ::common::UniqueId* response,
      ::google::protobuf::Closure* done)
      {
         std::string id = to_string(generator());

         Goal g = fromProto(*request);
         g.id = id;
         g.achieved = false;

         std::shared_ptr<impl::Goal> newGoal(new impl::Goal(
            props,
            mContainer.acquireSlot(ItemTraits<impl::Goal>::CONTAINER_NAME)));

         mGoals.insert(std::make_pair(g.id, newGoal));
         raiseGoalChangedEvent(g.id);
         newGoal.OnAchievedChanged.connect(std::bind(&StrategyServiceImpl::OnGoalAchivedChanged, this, g.id));
         
         auto parent = mGoals.find(g.parentGoalId);

         if(parent != mGoals.end())
         {
            parent->second->registerChild(newGoal);
         }

         response->set_guid(id);
      }

   void ModifyGoal(::google::protobuf::RpcController* controller,
      const ::strategy::Goal* request,
      ::common::OperationResultMessage* response,
      ::google::protobuf::Closure* done)
      {
         response->set_success(false);

         auto newGoalProps = fromProto(*request);

         Id id(request->id());
         auto g = mGoals.find(id);
         if(g != mGoals.end())
         {
            auto goal = g->second;

            auto oldParent = goal->getProps().parentGoalId;
            auto newParent = newGoalProps.parentGoalId;
            if(oldParent != newParent)
            {
               reparent(goal, oldParent, newParent);
            }

            g->second->accept(newGoalProps);
            raiseGoalChangedEvent(id);

            response->set_success(true);
         }
      }

   void DeleteGoal(::google::protobuf::RpcController* controller,
      const ::common::UniqueId* request,
      ::common::OperationResultMessage* response,
      ::google::protobuf::Closure* done)
      {
         response->set_success(false);
         
         Id id(*request);

         auto goal = mGoals.find(id);
         if(goal != mGoals.end())
         {
            deleteGoalImpl(id);
            response->set_success(true);
         }
      }

   void GetGoals(::google::protobuf::RpcController* controller,
      const ::common::EmptyMessage* request,
      ::strategy::Goals* response,
      ::google::protobuf::Closure* done)
      {
         for(auto x : mGoals)
         {
            response->add_items()->CopyFrom(toProto(x->getProps()));
         }
      }

   void GetGoal(::google::protobuf::RpcController* controller,
      const ::common::UniqueId* request,
      ::strategy::Goal* response,
      ::google::protobuf::Closure* done)
      {
         Id id(*request);

         auto iter = mGoals.find(id);
         if(iter != mGoals.end())
         {
            response->CopyFrom(toProto(*iter));
         }
      }

   void AddObjective(::google::protobuf::RpcController* controller,
      const ::strategy::Objective* request,
      ::common::UniqueId* response,
      ::google::protobuf::Closure* done)
      {
         /*std::string id = to_string(generator());

         Objective o = fromProto(*request);
         o.id = id;
         
         auto parent = mGoalTree.find(o.parentGoalId);

         if(parent != mGoalTree.end())
         {
            if(o.measurementId != Id::Invalid)
            {
               o.reached = calculateReached(o.measurementId, o.expected);
            }
            mObjectives.add(o);
            mEventRaiser.raiseGoalChangedEvent(o.parentGoalId);
            mGoalTree.recalculateAchieved(parent);
            response->set_guid(id);
         }*/
      }

   void ModifyObjective(::google::protobuf::RpcController* controller,
      const ::strategy::Objective* request,
      ::common::OperationResultMessage* response,
      ::google::protobuf::Closure* done)
      {
         /*response->set_success(false);
         Objective o = fromProto(*request);

         if(!mObjectives.contains(o.id))
         {
            return;
         }
         
         auto parent = mGoalTree.find(o.parentGoalId);

         if(parent != mGoalTree.end())
         {
            if(o.measurementId != Id::Invalid)
            {
               o.reached = calculateReached(o.measurementId, o.expected);
            }
            mObjectives.update(o);
            mEventRaiser.raiseGoalChangedEvent(o.parentGoalId);
            mGoalTree.recalculateAchieved(parent);
            response->set_success(true);
         }*/
      }

   void DeleteObjective(::google::protobuf::RpcController* controller,
      const ::common::UniqueId* request,
      ::common::OperationResultMessage* response,
      ::google::protobuf::Closure* done)
      {
         /*response->set_success(false);
         Objective o = fromProto(*request);
         
         auto parent = mGoalTree.find(o.parentGoalId);

         if(parent != mGoalTree.end())
         {
            mObjectives.erase(o);
            mEventRaiser.raiseGoalChangedEvent(o.parentGoalId);
            mGoalTree.recalculateAchieved(parent);
            response->set_success(true);
         }*/
      }

   void AddTask(::google::protobuf::RpcController* controller,
      const ::strategy::Task* request,
      ::common::UniqueId* response,
      ::google::protobuf::Closure* done)
      {
         std::string id = to_string(generator());

         Task t = fromProto(*request);
         t.id = id;
         
         auto parent = mGoals.find(t.parentGoalId);

         if(parent != mGoals.end())
         {
            mTasks.insert(t);
            raiseGoalChangedEvent(t.parentGoalId);
            response->set_guid(id);
         }
      }

   void ModifyTask(::google::protobuf::RpcController* controller,
      const ::strategy::Task* request,
      ::common::OperationResultMessage* response,
      ::google::protobuf::Closure* done)
      {
         response->set_success(false);
         auto newTask = fromProto(*request);
         auto oldTask = mTasks.find(newTask.id);

         if(oldTask != mTasks->end())
         {
            auto oldParent = mGoals.find(oldTask->parentGoalId);
            auto newParent = mGoals.find(newTask.parentGoalId);

            if(newParent != mGoals.end())
            {
               if(newParent != oldParent)
               {
                  oldParent.raiseGoalChangedEvent(oldParent->parentGoalId);
               }
               mTasks.update(newTask);
               raiseGoalChangedEvent(newParent->parentGoalId);
               response->set_success(true);
            }
         }
      }

   void DeleteTask(::google::protobuf::RpcController* controller,
      const ::common::UniqueId* request,
      ::common::OperationResultMessage* response,
      ::google::protobuf::Closure* done)
      {
         response->set_success(false);
         Task t = fromProto(*request);
         
         auto parent = mGoals.find(t.parentGoalId);
         auto tPos = mTasks.find(t.id);

         if(parent != mGoals.end() && tPos != mTasks.end())
         {
            mTasks.erase(t);
            raiseGoalChangedEvent(tPos.parentGoalId);
            response->set_success(true);
         }
      }

   void GetGoalItems(::google::protobuf::RpcController* controller,
      const ::common::UniqueId* request,
      ::strategy::GoalItems* response,
      ::google::protobuf::Closure* done)
      {
         Id id(*request);

         //auto obs = getGoalItems(mObjectives, id);

         /*for(auto x : obs)
         {
            response->add_objectives()->CopyFrom(toProto(x));
         }*/

         for(auto x : mTasks)
         {
            if(x.parentGoalId == id)
            {
               response->add_tasks()->CopyFrom(toProto(x));
            }
         }
      }

   void AddMeasurement(::google::protobuf::RpcController* controller,
      const ::strategy::Measurement* request,
      ::common::UniqueId* response,
      ::google::protobuf::Closure* done)
      {
         std::string id = to_string(generator());
         auto props = fromProto(*request);
         props.id = id; 

         std::shared_ptr<impl::Measurement> newMeas(new impl::Measurement(
            props,
            mContainer.acquireSlot(ItemTraits<impl::Measurement>::CONTAINER_NAME)));
         
         mMeasurements.insert(std::make_pair(id, newMeas));
         raiseMeasurementChangedEvent(id);
         response->set_guid(id);
      }

   void ModifyMeasurement(::google::protobuf::RpcController* controller,
      const ::strategy::Measurement* request,
      ::common::OperationResultMessage* response,
      ::google::protobuf::Closure* done)
      {
         response->set_success(false);

         Id id(request->id());
         auto meas = mMeasurements.find(id);
         if(meas != mMeasurements.end())
         {
            meas->second->accept(fromProto(*request));
            raiseMeasurementChangedEvent(id);

            response->set_success(true);
         }
      }

   void DeleteMeasurement(::google::protobuf::RpcController* controller,
      const ::common::UniqueId* request,
      ::common::OperationResultMessage* response,
      ::google::protobuf::Closure* done)
      {
         response->set_success(false);
         
         Id id(*request);

         auto meas = mMeasurements.find(id);
         if(meas != mMeasurements.end())
         {
            mMeasurements.erase(id);
            raiseMeasurementChangedEvent(id);
            response->set_success(true);
         }
      }

   void GetMeasurements(::google::protobuf::RpcController* controller,
      const ::common::EmptyMessage* request,
      ::strategy::Measurements* response,
      ::google::protobuf::Closure* done)
      {
         for(auto x : mMeasurements)
         {
            response->add_items()->CopyFrom(toProto(x.second->getProps()));
         }
      }

   void ConfigureAffinities(::google::protobuf::RpcController* controller,
      const ::strategy::Affinities* request,
      ::common::OperationResultMessage* response,
      ::google::protobuf::Closure* done)
      {
         mAffinities.clear();

         std::vector<Affinity> result(request->items_size());
         std::transform(request->items().begin(), request->items().end(), result.begin(), [] (auto x)-> auto { return fromProto(x); });

         mAffinities.insert(result.begin(), result.end());
         raiseAffinitiesChangedEvent();
         response->set_success(true);
      }

   void GetAffinities(::google::protobuf::RpcController* controller,
      const ::common::EmptyMessage* request,
      ::strategy::Affinities* response,
      ::google::protobuf::Closure* done)
      {
         for(auto x : mAffinities)
         {
            response->add_items()->CopyFrom(toProto(x));
         }
      }

   void Clear(::google::protobuf::RpcController* controller,
      const ::common::EmptyMessage* request,
      ::common::OperationResultMessage* response,
      ::google::protobuf::Closure* done)
      {
         mGoals.clear();
         mTasks.clear();
         mObjectives.clear();
         mMeasurements.clear();
         mAffinities.clear();
      }

private:
   template<class T>
   using TItems = std::map<Id, std::shared_ptr<T>>;

   void deleteGoalImpl(const Id& id)
   {
      auto g = mGoals[id]->second;
      auto parentId = g.getProps().parentGoalId;
      
      if(parentId != Id::Invalid)
      {
         mGoals[parentId]->second->unregisterChild(g);
      }

      auto children = g->getChildren();
      for(auto x : children)
      {
         deleteGoalImpl(x->getProps().id);
      }

      mGoals.erase(id);
      raiseGoalChangedEvent(id);
   }

   void reparent(std::shared_ptr<impl::Goal>& g, const Id& oldParent, const Id& newParent)
   {
      if(oldParent != Id::Invalid)
      {
         mGoals[oldParent]->unregisterChild(g);
      }
      if(newParent != Id::Invalid)
      {
         auto newParentGoal = mGoals.find(newParent);
         if(newParentGoal != mGoals.end())
         {
            newParentGoal->second->registerChild(g);
         }
      }
   }

   void OnGoalAchivedChanged(const Id& goalId)
   {
      raiseGoalChangedEvent(g.id);
   }

   std::vector<ContainerItem> loadOrCreateContainer(const std::string& name)
   {
      auto items = mContainer.getItems(name);
      if(items.empty())
      {
         mContainer.addContainer({name, false});
      }

      return items;
   }

   template<class T>
   void populateCollection(TItems<T>& collection)
   {
      auto items = loadOrCreateContainer(ItemTraits<T>::CONTAINER_NAME);

      for(auto x : items)
      {
         std::shared_ptr<T> newItem(new T(
            x.content,
            mContainer.acquireSlot(ItemTraits<T>::CONTAINER_NAME, x.id)));
         
         collection.insert(std::make_pair(newItem->getProps().id, newItem));
      }
   }

   void connectGoals()
   {
      for(auto g : mGoals)
      {
         auto parentGoalId = g->getProps().parentGoalId;
         if(parentGoalId != Id::Invalid)
         {
            mGoals[parentGoalId]->registerChild(g);
         }
      }
   }

   void connectMeasurementsWithObjectives()
   {
      for(auto o : mObjectives)
      {
         auto measId = o->getProps().measId;
         if(measId != Id::Invalid)
         {
            o->connect(*mMeasurements[measId]);
         }
      }
   }

   void connectObjectivesWithGoals()
   {
      for(auto o : mObjectives)
      {
         mGoals[o->getProps().parentGoalId]->connect(*o);
      }
   }

   void raiseMeasurementChangedEvent(const Id& id)
   {
      materia::IdEvent idEv;
      idEv.type = materia::EventType::MeasurementUpdated;
      idEv.timestamp = boost::posix_time::second_clock::local_time();
      idEv.id = id;

      mEvents.putEvent(idEv);
   }

   void raiseGoalChangedEvent(const Id& id)
   {
      materia::IdEvent idEv;
      idEv.type = materia::EventType::GoalUpdated;
      idEv.timestamp = boost::posix_time::second_clock::local_time();
      idEv.id = id;

      mEvents.putEvent(idEv);
   }

   void raiseAffinitiesChangedEvent()
   {
      materia::Event idEv;
      idEv.type = materia::EventType::AffinitiesUpdated;
      idEv.timestamp = boost::posix_time::second_clock::local_time();

      mEvents.putEvent(idEv);
   }

   TItems<impl::Goal> mGoals;
   TItems<impl::Measurement> mMeasurements;
   TItems<impl::Objective> mObjectives;

   Container& mContainer;
   Events& mEvents;
   
   RemoteCollection<Task> mTask;
   RemoteCollection<Affinity> mAffinities;
};

}

int main(int argc, char *argv[])
{
   materia::MateriaClient client("StrategyService");

   materia::StrategyServiceImpl serviceImpl(client.getContainer(), client.getEvents());
   materia::InterprocessService<materia::StrategyServiceImpl> service(serviceImpl);
   
   service.provideAt("*:" + gStrategyPort, "StrategyService");
   
   return 0;
}
