#include <iostream>
#include <fstream>
#include <Common/InterprocessService.hpp>
#include <Common/PortLayout.hpp>
#include <Client/MateriaClient.hpp>
#include <messages/strategy.pb.h>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include "tree.hh"

//17

namespace materia
{

boost::uuids::random_generator generator;

typedef std::map<Id, std::vector<Objective>> TGoalToObjectivesMap;

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

class GoalStorage
{
public:
   GoalStorage(Container& container)
   : mContainer(container)
   {

   }

   void create(const Goal& g)
   {
      mContainer.addContainer({g.id.getGuid(), false});
      auto insertedIds = mContainer.insertItems("goals", {{ materia::Id::Invalid, toJson(g) }});
      if(insertedIds.size() == 1)
      {
         mGoalToContainerItemMap.insert(std::make_pair(g.id, insertedIds[0]));
      }
   }

   void update(const Goal& g)
   {
      mContainer.replaceItems("goals", {{ mGoalToContainerItemMap[g.id], toJson(g) }});
   }

   std::vector<Goal> restore();

private:
   std::string toJson(const Goal& g);

   std::map<Id, Id> mGoalToContainerItemMap;
   Container& mContainer;
};

class GoalTree
{
public:
   GoalTree(
      GoalStorage goalStorage, 
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
      bool operator == (const Iterator& other) const;
      bool operator != (const Iterator& other) const;

      const Goal* operator -> () const;

   protected:
      tree<Goal>::iterator_base mRawIter; 
   };

   Iterator begin() const;
   Iterator end() const;
   Iterator find(const Id& id) const;

   void addChild(const Iterator& parentIter, const Goal& item)
   {
      auto parentItem = getItem(parentIter);
      recalculateAchieved(item, parentItem);

      mImpl.append_child(parentIter, item);

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
         auto newParentIter = find(newItem.parentGoalId).mRawIter;

         mEventRaiser.raiseGoalChangedEvent(oldItem.parentGoalId);
         mEventRaiser.raiseGoalChangedEvent(newItem.parentGoalId);

         auto subTree = mImpl.move_out(pos.mRawIter);
         mImpl.move_in(mImpl.begin(newParentIter), subTree);

         recalculateAchieved(newItem, *newParentIter);
      }

      oldItem = newItem;

      mGoalStorage.update(oldItem);
   }

   void erase(const Iterator& pos)
   {

   }

private:

   Goal& getItem(const Iterator& iter)
   {
      return *(iter.mRawIter);
   }

   bool isSimple(const Goal& g)
   {
      return tree<Goal>::number_of_children(find(g.id).mRawIter) == 0 ||
         mGoalToObjectivesMap.find(g.id) == mGoalToObjectivesMap.end();
   }

   void recalculateAchieved(const Goal& newChild, Goal& item)
   {
      bool oldAchieved = item.achieved;

      if(isSimple(item))
      {
         item.achieved = newChild.achieved;
      }
      else
      {
         item.achieved = oldAchieved && newChild.achieved;
      }

      if(oldAchieved != item.achieved)
      {
         auto nextParentIter = find(item.parentGoalId);

         if(nextParentIter != end())
         {
            recalculateAchieved(item, getItem(nextParentIter));
            mEventRaiser.raiseGoalChangedEvent(item.id);
            mGoalStorage.update(item);
         }
      }
   }

   GoalStorage mGoalStorage;
   const TGoalToObjectivesMap& mGoalToObjectivesMap;
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
      ::google::protobuf::Closure* done) //finished
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
      ::google::protobuf::Closure* done) //finished
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
      ::google::protobuf::Closure* done) //finished
      {
         response->set_success(false);

         Id id(*request);

         auto iter = mGoalTree.find(id);
         if(iter != mGoalTree.end())
         {
            mGoalTree.erase(iter); //raise event for parent, recalculate achieved
            response->set_success(true);
         }
      }

   void GetGoals(::google::protobuf::RpcController* controller,
      const ::common::EmptyMessage* request,
      ::strategy::Goals* response,
      ::google::protobuf::Closure* done)
      {
         
      }

   void AddObjective(::google::protobuf::RpcController* controller,
      const ::strategy::Objective* request,
      ::common::UniqueId* response,
      ::google::protobuf::Closure* done)
      {
         
      }

   void ModifyObjective(::google::protobuf::RpcController* controller,
      const ::strategy::Objective* request,
      ::common::OperationResultMessage* response,
      ::google::protobuf::Closure* done)
      {
         
      }

   void DeleteObjective(::google::protobuf::RpcController* controller,
      const ::common::UniqueId* request,
      ::common::OperationResultMessage* response,
      ::google::protobuf::Closure* done)
      {
         
      }

   void AddTask(::google::protobuf::RpcController* controller,
      const ::strategy::Task* request,
      ::common::UniqueId* response,
      ::google::protobuf::Closure* done)
      {

      }

   void ModifyTask(::google::protobuf::RpcController* controller,
      const ::strategy::Task* request,
      ::common::OperationResultMessage* response,
      ::google::protobuf::Closure* done)
      {
         
      }

   void DeleteTask(::google::protobuf::RpcController* controller,
      const ::common::UniqueId* request,
      ::common::OperationResultMessage* response,
      ::google::protobuf::Closure* done)
      {
         
      }

   void GetGoalItems(::google::protobuf::RpcController* controller,
      const ::common::UniqueId* request,
      ::strategy::GoalItems* response,
      ::google::protobuf::Closure* done)
      {
         
      }

   void AddMeasurement(::google::protobuf::RpcController* controller,
      const ::strategy::Measurement* request,
      ::common::UniqueId* response,
      ::google::protobuf::Closure* done)
      {
         
      }

   void ModifyMeasurement(::google::protobuf::RpcController* controller,
      const ::strategy::Measurement* request,
      ::common::OperationResultMessage* response,
      ::google::protobuf::Closure* done)
      {
         
      }

   void DeleteMeasurement(::google::protobuf::RpcController* controller,
      const ::common::UniqueId* request,
      ::common::OperationResultMessage* response,
      ::google::protobuf::Closure* done)
      {

      }

   void GetMeasurements(::google::protobuf::RpcController* controller,
      const ::common::EmptyMessage* request,
      ::strategy::Measurements* response,
      ::google::protobuf::Closure* done)
      {

      }

   void ConfigureAffinities(::google::protobuf::RpcController* controller,
      const ::strategy::Affinities* request,
      ::common::OperationResultMessage* response,
      ::google::protobuf::Closure* done)
      {

      }

   void GetAffinities(::google::protobuf::RpcController* controller,
      const ::common::EmptyMessage* request,
      ::strategy::Affinities* response,
      ::google::protobuf::Closure* done)
      {

      }

   void Clear(::google::protobuf::RpcController* controller,
      const ::common::EmptyMessage* request,
      ::common::OperationResultMessage* response,
      ::google::protobuf::Closure* done)
      {
         
      }

private:

   TGoalToObjectivesMap mGoalToObjectivesMap;
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
