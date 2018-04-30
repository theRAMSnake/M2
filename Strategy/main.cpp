#include <iostream>
#include <fstream>
#include <Common/InterprocessService.hpp>
#include <Common/PortLayout.hpp>
#include <Client/MateriaClient.hpp>
#include <messages/strategy.pb.h>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace materia
{

boost::uuids::random_generator generator;

class StrategyServiceImpl : public strategy::StrategyService
{
public:
   StrategyServiceImpl()
   {
      loadGoalTree();
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

         if(parent && checkPrereqGoals(g))
         {
            auto item = mGoalTree.addChild(parent, g); //raise event for parent, recalculate achieved

            mContainer.createContainer(id, false);
            auto insertedIds = mContainer.insertItems("goals", {{ materia::Id::Invalid, toJson(g) }});
            if(insertedIds.size() == 1)
            {
               mGoalToContainerItemMap.insert(std::make_pair(g.id, insertedIds[0]);
               mEvents.add({g.id, EventType::GoalUpdated});
               responce->set_guid(id);
            }
         }
      }

   void ModifyGoal(::google::protobuf::RpcController* controller,
      const ::strategy::Goal* request,
      ::common::OperationResultMessage* response,
      ::google::protobuf::Closure* done)
      {
         response->set_success(false);

         Goal g = fromProto(*request);

         auto item = mGoalTree.find(g.id);

         if(item && checkPrereqGoals(g))
         {
            if(item->goal->parentGoalId != g.parentGoalId)
            {
               mGoalTree.move(g.id, g.parentGoalId); //raise event for parent (old and new), recalculate achieved
            }

            g.achieved = calculateAchieved(g.id);
            *item = g;
            if(mContainer.replaceItems("goals", {{ mGoalToContainerItemMap[g.id], toJson(g) }}))
            {
               mEvents.add({g.id, EventType::GoalUpdated});
               response->set_success(true);
            }
         }
      }

   void DeleteGoal(::google::protobuf::RpcController* controller,
      const ::common::UniqueId* request,
      ::common::OperationResultMessage* response,
      ::google::protobuf::Closure* done)
      {
         response->set_success(false);

         Goal g = fromProto(*request);

         auto item = mGoalTree.find(g.id);
         if(item)
         {
            mGoalTree.remove(g.id); //raise event for parent, recalculate achieved

            if(mContainer.deleteItems("goals", {{ mGoalToContainerItemMap[g.id] }}))
            {
               removeTasksAndObjectives(g.id);
               mContainer.deleteContainer(g.id);
               mGoalToContainerItemMap.erase(mGoalToContainerItemMap.find(g.id));
               
               mEvents.add({g.id, EventType::GoalUpdated});
               response->set_success(true);
            }
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
   
};

}

int main(int argc, char *argv[])
{
   materia::StrategyServiceImpl serviceImpl;
   materia::InterprocessService<materia::StrategyServiceImpl> service(serviceImpl);
   
   service.provideAt("*:" + gStrategyPort, "StrategyService");
   
   return 0;
}
