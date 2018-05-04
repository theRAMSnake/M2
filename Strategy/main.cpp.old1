#include <iostream>
#include <fstream>
#include <Common/InterprocessService.hpp>
#include <Common/PortLayout.hpp>
#include <Client/MateriaClient.hpp>
#include <messages/strategy.pb.h>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include "tree.hh"

namespace materia
{

boost::uuids::random_generator generator;

class EventRaiser
{
public:
   EventRaiser(Events& events)
   : mEvents(events)
   {

   }

   void raiseGoalChangedEvent(const Id& id)
   {
      namespace pt = boost::posix_time;

      IdEvent ev;
      ev.type = EventType::GoalUpdated;
      ev.id = id;
      ev.timestamp = pt::second_clock::local_time();

      mEvents.putEvent<IdEvent>(ev);
   }

private:
   Events& mEvents;
};

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
}

template<class T>
class RemoteCollection
{
public:
   RemoteCollection(const std::string& name, Container& container)
   : mName(name)
   , mContainer(container)
   {
      auto items = mContainer.getItems(mName);

      mLocalCache.reserve(items.size());

      for(auto x : items)
      {
         auto item = fromJson(x.content);
         mLocalToRemoteIdMap.insert(std::make_pair(item.id, x.id));
         mLocalCache.push_back(item);
         result.push_back(item);
      }

      return result;
   }

   void insert(const T& item)
   {
      auto insertedIds = mContainer.insertItems(mName, {{ materia::Id::Invalid, toJson(item) }});
      if(insertedIds.size() == 1)
      {
         mLocalToRemoteIdMap.insert(std::make_pair(item.id, insertedIds[0]));
         mLocalCache.push_back(item);
      }
   }

   void update(const T& item)
   {
      mContainer.replaceItems(mName, {{ mLocalToRemoteIdMap[item.id], toJson(g) }});
      *find(mLocalCache, item.id) = item;
   }

   const std::vector<T>& getItems()
   {  
      return mLocalCache;
   }

private:

   std::vector<T> mLocalCache;
   std::map<Id, Id> mLocalToRemoteIdMap;
   std::string mName;
   Container& mContainer;
};

template<class T>
std::vector<T> getGoalItems(const RemoteCollection<T>& collection, const Id& id)
{
   std::vector<T> result;

   for(auto x : collection.getItems())
   {
      if(x.parentGoalId == id)
      {
         result.push_back(x);
      }
   }

   return result;
}

class GoalTree
{
public:
   GoalTree(
      RemoteCollection<Goal> goalStorage, 
      const TGoalToObjectivesMap& goalToObjectivesMap,
      EventRaiser& eventRaiser
      )
   : mGoalStorage(goalStorage)
   , mGoalToObjectivesMap(goalToObjectivesMap)
   , mEventRaiser(eventRaiser)
   {
      mImpl.insert(mImpl.begin(), Goal());

      for(auto x : mGoalStorage.restore())
      {
         mImpl.append_child(find(x.parentGoalId).mRawIter, x);
      }
   }

   class Iterator
   {
      friend class GoalTree;

   public:
      Iterator(tree<Goal>::iterator init)
      : mRawIter(init)
      {

      }

      bool operator == (const Iterator& other) const
      {
         return mRawIter == other.mRawIter;
      }

      bool operator != (const Iterator& other) const
      {
         return !operator == (other);
      }

      const Goal* operator -> () const
      {
         return *mRawIter;
      }

   protected:
      tree<Goal>::iterator mRawIter; 
   };

   Iterator begin() const
   {
      return Iterator(mImpl.begin());
   }

   Iterator end() const
   {
      return Iterator(mImpl.end());
   }

   Iterator find(const Id& id) const
   {
      return Iterator(std::find_if(mImpl.begin(), mImpl.end(), [=](auto x)->bool{return x.id == id;}));
   }

   void addChild(const Iterator& parentIter, const Goal& item)
   {
      auto parentItem = getItem(parentIter);

      mImpl.append_child(parentIter, item);

      recalculateAchieved(parentItem);

      mEventRaiser.raiseGoalChangedEvent(parentIter->id);
      mEventRaiser.raiseGoalChangedEvent(item.id);

      mGoalStorage.create(item);
   }

   void setItem(const Iterator& pos, Goal& newItem)
   {
      auto oldItem = getItem(pos);

      newItem.achieved = oldItem.achieved;

      if(oldItem.parentGoalId != newItem.parentGoalId)
      {
         auto newParentIter = find(newItem.parentGoalId);

         mEventRaiser.raiseGoalChangedEvent(oldItem.parentGoalId);
         mEventRaiser.raiseGoalChangedEvent(newItem.parentGoalId);

         auto subTree = mImpl.move_out(pos.mRawIter);
         mImpl.move_in(mImpl.begin(newParentIter.mRawIter), subTree);

         recalculateAchieved(newParentIter);
      }

      oldItem = newItem;

      mGoalStorage.update(oldItem);
   }

   void erase(const Iterator& pos)
   {
      auto item = getItem(pos);

      //Can never be 'end' in correct tree
      auto parent = find(item.parentGoalId);

      auto subTree = mImpl.move_out(pos.mRawIter);

      recalculateAchieved(parent);

      for(auto x : subTree)
      {
         mGoalStorage.erase(x.id);
         mEventRaiser.raiseGoalChangedEvent(x.id);
      }
   }

   void recalculateAchieved(const Iterator& iter)
   {
      auto item = getItem(iter);

      if(item.id == Id::Invalid)
      {
         return;
      }

      bool oldAchieved = item.achieved;

      auto childrenBegin = mImpl.begin(iter);
      auto childrenEnd = mImpl.end(iter);

      auto objectives = getGoalItems(mObjectives, item.id);
      if(childrenBegin == childrenEnd && objectives.empty())
      {
         item.achieved = false;
      }
      else
      {
         item.achieved = true;
         for(auto it = childrenBegin; it != childrenEnd; ++it)
         {
            if(!it->achieved)
            {
               item.achieved = false;
               break;
            }
         }

         if(item.achieved)
         {
            for(auto x : objectives)
            {
               if(!x.reached)
               {
                  item.achieved = false;
                  break;
               }
            }
         }
      }

      if(oldAchieved != item.achieved)
      {
         auto nextParentIter = find(item.parentGoalId);

         if(nextParentIter != end())
         {
            recalculateAchieved(nextParentIter);
            mEventRaiser.raiseGoalChangedEvent(item.id);
            mGoalStorage.update(item);
         }
      }
   }

private:

   RemoteCollection<Goal> mGoalStorage;
   EventRaiser& mEventRaiser;
   tree<Goal> mImpl;
};

class StrategyServiceImpl : public strategy::StrategyService
{
public:
   StrategyServiceImpl(Container& container, Events& events)
   : mEventRaiser(events)
   , mGoalTree(GoalStorage(container), mGoalToObjectivesMap, mEventRaiser)
   {
      
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
         
         auto parent = mGoalTree.find(g.parentGoalId);

         if(parent != mGoalTree.end())
         {
            mGoalTree.addChild(parent, g);
            response->set_guid(id);
         }
      }

   void ModifyGoal(::google::protobuf::RpcController* controller,
      const ::strategy::Goal* request,
      ::common::OperationResultMessage* response,
      ::google::protobuf::Closure* done)
      {
         response->set_success(false);

         Goal g = fromProto(*request);

         auto iter = mGoalTree.find(g.id);

         if(iter != mGoalTree.end())
         {
            mGoalTree.setItem(iter, g);
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

         auto iter = mGoalTree.find(id);
         if(iter != mGoalTree.end())
         {
            mGoalTree.erase(iter); 
            response->set_success(true);

            mObjectives.erase(getGoalItems(mObjectives, id));
            mTasks.erase(getGoalItems(mTasks, id));
         }
      }

   void GetGoals(::google::protobuf::RpcController* controller,
      const ::common::EmptyMessage* request,
      ::strategy::Goals* response,
      ::google::protobuf::Closure* done)
      {
         for(auto x : mGoalTree)
         {
            response->add_items()->CopyFrom(toProto(x));
         }
      }

   void GetGoal(::google::protobuf::RpcController* controller,
      const ::common::UniqueId* request,
      ::strategy::Goal* response,
      ::google::protobuf::Closure* done)
      {
         Id id(*request);

         auto iter = mGoalTree.find(id);
         if(iter != mGoalTree.end())
         {
            response->CopyFrom(toProto(*iter));
         }
      }

   void AddObjective(::google::protobuf::RpcController* controller,
      const ::strategy::Objective* request,
      ::common::UniqueId* response,
      ::google::protobuf::Closure* done)
      {
         std::string id = to_string(generator());

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
         }
      }

   void ModifyObjective(::google::protobuf::RpcController* controller,
      const ::strategy::Objective* request,
      ::common::OperationResultMessage* response,
      ::google::protobuf::Closure* done)
      {
         response->set_success(false);
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
         }
      }

   void DeleteObjective(::google::protobuf::RpcController* controller,
      const ::common::UniqueId* request,
      ::common::OperationResultMessage* response,
      ::google::protobuf::Closure* done)
      {
         response->set_success(false);
         Objective o = fromProto(*request);
         
         auto parent = mGoalTree.find(o.parentGoalId);

         if(parent != mGoalTree.end())
         {
            mObjectives.erase(o);
            mEventRaiser.raiseGoalChangedEvent(o.parentGoalId);
            mGoalTree.recalculateAchieved(parent);
            response->set_success(true);
         }
      }

   void AddTask(::google::protobuf::RpcController* controller,
      const ::strategy::Task* request,
      ::common::UniqueId* response,
      ::google::protobuf::Closure* done)
      {
         std::string id = to_string(generator());

         Task t = fromProto(*request);
         t.id = id;
         
         auto parent = mGoalTree.find(t.parentGoalId);

         if(parent != mGoalTree.end())
         {
            mTasks.add(t);
            mEventRaiser.raiseGoalChangedEvent(t.parentGoalId);
            response->set_guid(id);
         }
      }

   void ModifyTask(::google::protobuf::RpcController* controller,
      const ::strategy::Task* request,
      ::common::OperationResultMessage* response,
      ::google::protobuf::Closure* done)
      {
         response->set_success(false);
         Task t = fromProto(*request);
         
         auto parent = mGoalTree.find(t.parentGoalId);

         if(parent != mGoalTree.end())
         {
            mTasks.update(t);
            mEventRaiser.raiseGoalChangedEvent(t.parentGoalId);
            response->set_success(true);
         }
      }

   void DeleteTask(::google::protobuf::RpcController* controller,
      const ::common::UniqueId* request,
      ::common::OperationResultMessage* response,
      ::google::protobuf::Closure* done)
      {
         response->set_success(false);
         Task t = fromProto(*request);
         
         auto parent = mGoalTree.find(t.parentGoalId);

         if(parent != mGoalTree.end())
         {
            mTasks.erase(t);
            mEventRaiser.raiseGoalChangedEvent(t.parentGoalId);
            response->set_success(true);
         }
      }

   void GetGoalItems(::google::protobuf::RpcController* controller,
      const ::common::UniqueId* request,
      ::strategy::GoalItems* response,
      ::google::protobuf::Closure* done)
      {
         Id id(*request);

         auto obs = getGoalItems(mObjectives, id);
         auto tasks = getGoalItems(mTasks, id);

         for(auto x : obs)
         {
            response->add_objectives()->CopyFrom(toProto(x));
         }

         for(auto x : tasks)
         {
            response->add_tasks()->CopyFrom(toProto(x));
         }
      }

   void AddMeasurement(::google::protobuf::RpcController* controller,
      const ::strategy::Measurement* request,
      ::common::UniqueId* response,
      ::google::protobuf::Closure* done)
      {
         std::string id = to_string(generator());

         Measurement m = fromProto(*request);
         m.id = id;
         
         mMeasurements.add(m);
         mEventRaiser.raiseMeasurementChangedEvent(m.id);
         response->set_guid(id);
      }

   void ModifyMeasurement(::google::protobuf::RpcController* controller,
      const ::strategy::Measurement* request,
      ::common::OperationResultMessage* response,
      ::google::protobuf::Closure* done)
      {
         response->set_success(false);
         Measurement m = fromProto(*request);

         if(!mMeasurements.contains(m.id))
         {
            return;
         }

         for(auto x : mObjectives.getItems())
         {
            if(x.measurementId == m.id)
            {
               x.reached = calculateReached(m, x.expected);
               mObjectives.update(x);

               auto parent = mGoalTree.find(x.parentGoalId);
               mGoalTree.recalculateAchieved(parent);
               mEventRaiser.raiseMeasurementChangedEvent(m.id);
            }
         }

         response->set_success(true);
      }

   void DeleteMeasurement(::google::protobuf::RpcController* controller,
      const ::common::UniqueId* request,
      ::common::OperationResultMessage* response,
      ::google::protobuf::Closure* done)
      {
         response->set_success(false);
         Measurement m = fromProto(*request);
         
         mMeasurements.erase(m);
         mEventRaiser.raiseMeasurementChangedEvent(m.id);
      }

   void GetMeasurements(::google::protobuf::RpcController* controller,
      const ::common::EmptyMessage* request,
      ::strategy::Measurements* response,
      ::google::protobuf::Closure* done)
      {
         for(auto x : mMeasurements.getItems())
         {
            response->add_items()->CopyFrom(toProto(x));
         }
      }

   void ConfigureAffinities(::google::protobuf::RpcController* controller,
      const ::strategy::Affinities* request,
      ::common::OperationResultMessage* response,
      ::google::protobuf::Closure* done)
      {
         std::vector<Affinity> result(request.items_size());
         std::transform(request.items().begin(), request.items().end(), result.begin(), [] (auto x)-> auto { return fromProto(x); });

         mAffinities.reset(result);
         mEventRaiser.raiseAffinitiesChangedEvent();
         response->set_success(true);
      }

   void GetAffinities(::google::protobuf::RpcController* controller,
      const ::common::EmptyMessage* request,
      ::strategy::Affinities* response,
      ::google::protobuf::Closure* done)
      {
         for(auto x : mAffinities.getItems())
         {
            response.add_items()->CopyFrom(toProto(x));
         }
      }

   void Clear(::google::protobuf::RpcController* controller,
      const ::common::EmptyMessage* request,
      ::common::OperationResultMessage* response,
      ::google::protobuf::Closure* done)
      {
         mGoalTree.clear();
         mObjectives.clear();
         mTasks.clear();
         mMeasurements.clear();
         mAffinities.clear();
      }

private:

   RemoteCollection<Objective> mObjectives;
   RemoteCollection<Task> mTasks;
   RemoteCollection<Measurement> mMeasurements;
   RemoteCollection<Affinity> mAffinities;
   EventRaiser mEventRaiser;
   GoalTree mGoalTree;
};

}

int main(int argc, char *argv[])
{
   materia::StrategyServiceImpl serviceImpl;
   materia::InterprocessService<materia::StrategyServiceImpl> service(serviceImpl);
   
   service.provideAt("*:" + gStrategyPort, "StrategyService");
   
   return 0;
}
