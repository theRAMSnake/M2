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

      OnValueChanged(mImpl.value);
   }

   const materia::Measurement& getProps()
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

class Goal
{
public:
   Goal(const materia::Goal& props, ContainerSlot&& slot);
   Goal(const std::string& json, ContainerSlot&& slot);
   const materia::Goal& getProps();
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

class StrategyServiceImpl : public strategy::StrategyService
{
public:
   StrategyServiceImpl(Container& container, Events& events)
   : mContainer(container)
   , mEvents(events)
   , mAffinities("affinities", container)
   {
      populateCollection(mGoals);
      populateCollection(mMeasurements);
   }

   void AddGoal(::google::protobuf::RpcController* controller,
      const ::strategy::Goal* request,
      ::common::UniqueId* response,
      ::google::protobuf::Closure* done)
      {
         /*std::string id = to_string(generator());

         Goal g = fromProto(*request);
         g.id = id;
         g.achieved = false;
         
         auto parent = mGoalTree.find(g.parentGoalId);

         if(parent != mGoalTree.end())
         {
            mGoalTree.addChild(parent, g);
            response->set_guid(id);
         }*/
      }

   void ModifyGoal(::google::protobuf::RpcController* controller,
      const ::strategy::Goal* request,
      ::common::OperationResultMessage* response,
      ::google::protobuf::Closure* done)
      {
         /*response->set_success(false);

         Goal g = fromProto(*request);

         auto iter = mGoalTree.find(g.id);

         if(iter != mGoalTree.end())
         {
            mGoalTree.setItem(iter, g);
            response->set_success(true);
         }*/
      }

   void DeleteGoal(::google::protobuf::RpcController* controller,
      const ::common::UniqueId* request,
      ::common::OperationResultMessage* response,
      ::google::protobuf::Closure* done)
      {
         /*response->set_success(false);

         Id id(*request);

         auto iter = mGoalTree.find(id);
         if(iter != mGoalTree.end())
         {
            mGoalTree.erase(iter); 
            response->set_success(true);

            mObjectives.erase(getGoalItems(mObjectives, id));
            mTasks.erase(getGoalItems(mTasks, id));
         }*/
      }

   void GetGoals(::google::protobuf::RpcController* controller,
      const ::common::EmptyMessage* request,
      ::strategy::Goals* response,
      ::google::protobuf::Closure* done)
      {
         /*for(auto x : mGoalTree)
         {
            response->add_items()->CopyFrom(toProto(x));
         }*/
      }

   void GetGoal(::google::protobuf::RpcController* controller,
      const ::common::UniqueId* request,
      ::strategy::Goal* response,
      ::google::protobuf::Closure* done)
      {
         /*Id id(*request);

         auto iter = mGoalTree.find(id);
         if(iter != mGoalTree.end())
         {
            response->CopyFrom(toProto(*iter));
         }*/
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
         /*std::string id = to_string(generator());

         Task t = fromProto(*request);
         t.id = id;
         
         auto parent = mGoalTree.find(t.parentGoalId);

         if(parent != mGoalTree.end())
         {
            mTasks.add(t);
            mEventRaiser.raiseGoalChangedEvent(t.parentGoalId);
            response->set_guid(id);
         }*/
      }

   void ModifyTask(::google::protobuf::RpcController* controller,
      const ::strategy::Task* request,
      ::common::OperationResultMessage* response,
      ::google::protobuf::Closure* done)
      {
         /*response->set_success(false);
         Task t = fromProto(*request);
         
         auto parent = mGoalTree.find(t.parentGoalId);

         if(parent != mGoalTree.end())
         {
            mTasks.update(t);
            mEventRaiser.raiseGoalChangedEvent(t.parentGoalId);
            response->set_success(true);
         }*/
      }

   void DeleteTask(::google::protobuf::RpcController* controller,
      const ::common::UniqueId* request,
      ::common::OperationResultMessage* response,
      ::google::protobuf::Closure* done)
      {
         /*response->set_success(false);
         Task t = fromProto(*request);
         
         auto parent = mGoalTree.find(t.parentGoalId);

         if(parent != mGoalTree.end())
         {
            mTasks.erase(t);
            mEventRaiser.raiseGoalChangedEvent(t.parentGoalId);
            response->set_success(true);
         }*/
      }

   void GetGoalItems(::google::protobuf::RpcController* controller,
      const ::common::UniqueId* request,
      ::strategy::GoalItems* response,
      ::google::protobuf::Closure* done)
      {
         /*Id id(*request);

         auto obs = getGoalItems(mObjectives, id);
         auto tasks = getGoalItems(mTasks, id);

         for(auto x : obs)
         {
            response->add_objectives()->CopyFrom(toProto(x));
         }

         for(auto x : tasks)
         {
            response->add_tasks()->CopyFrom(toProto(x));
         }*/
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
         mMeasurements.clear();
         mAffinities.clear();
      }

private:
   template<class T>
   using TItems = std::map<Id, std::shared_ptr<T>>;

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

   void raiseMeasurementChangedEvent(const Id& id)
   {
      materia::IdEvent idEv;
      idEv.type = materia::EventType::MeasurementUpdated;
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

   Container& mContainer;
   Events& mEvents;
   
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