#pragma once

#include <messages/strategy.pb.h>
#include <Core/IStrategy.hpp>
#include "ProtoConvertion.hpp"

namespace materia
{

StrategyItem fromProto(const strategy::CommonItemProperties& x)
{
   return {fromProto(x.id()), x.name(), x.notes()};
}

Goal fromProto(const strategy::Goal& x)
{
   Goal result;

   static_cast<StrategyItem&>(result) = fromProto(x.common_props());
   result.focused = x.focused();
   result.achieved = x.achieved();

   return result;
}

Objective fromProto(const strategy::Objective& x)
{
   Objective result;

   static_cast<StrategyItem&>(result) = fromProto(x.common_props());
   result.expectedResourceValue = x.expectedtreshold();
   result.resourceId = fromProto(x.res_id());
   result.parentGoalId = fromProto(x.parent_goal_id());
   result.reached = x.reached();

   return result;
}

Resource fromProto(const strategy::Resource& x)
{
   return {fromProto(x.id()), x.name(), x.value()};
}

strategy::Resource toProto(const Resource& res)
{
   strategy::Resource result;

   result.mutable_id()->CopyFrom(toProto(res.id));
   result.set_name(res.name);
   result.set_value(res.value);

   return result;
}

Task fromProto(const strategy::Task& x)
{
   Task result;

   static_cast<StrategyItem&>(result) = fromProto(x.common_props());
   result.done = x.done();
   result.parentGoalId = fromProto(x.parent_goal_id());

   return result;
}

strategy::CommonItemProperties toProto(const StrategyItem& x)
{
   strategy::CommonItemProperties result;

   result.mutable_id()->CopyFrom(toProto(x.id));
   result.set_name(x.name);
   result.set_notes(x.notes);

   return result;
}

strategy::Goal toProto(const Goal& x)
{
   strategy::Goal result;

   result.mutable_common_props()->CopyFrom(toProto(static_cast<const StrategyItem&>(x)));
   result.set_achieved(x.achieved);
   result.set_focused(x.focused);

   return result;
}

strategy::Objective toProto(const Objective& x)
{
   strategy::Objective result;

   result.mutable_common_props()->CopyFrom(toProto(static_cast<const StrategyItem&>(x)));
   result.mutable_parent_goal_id()->CopyFrom(toProto(x.parentGoalId));
   result.set_reached(x.reached);
   result.mutable_res_id()->CopyFrom(toProto(x.resourceId));
   result.set_expectedtreshold(x.expectedResourceValue);

   return result;
}

strategy::Task toProto(const Task& x)
{
   strategy::Task result;

   result.mutable_common_props()->CopyFrom(toProto(static_cast<const StrategyItem&>(x)));
   result.mutable_parent_goal_id()->CopyFrom(toProto(x.parentGoalId));
   result.set_done(x.done);
   
   return result;
}

class StrategyServiceImpl : public strategy::StrategyService
{
public:
   StrategyServiceImpl(ICore& core)
   : mStrategy(core.getStrategy())
   {

   }

   virtual void AddGoal(::google::protobuf::RpcController* controller,
                       const ::strategy::Goal* request,
                       ::common::UniqueId* response,
                       ::google::protobuf::Closure* done)
   {
      response->CopyFrom(toProto(mStrategy.addGoal(fromProto(*request))));
   }

   virtual void ModifyGoal(::google::protobuf::RpcController* controller,
                       const ::strategy::Goal* request,
                       ::common::OperationResultMessage* response,
                       ::google::protobuf::Closure* done)
   {
      mStrategy.modifyGoal(fromProto(*request));
      response->set_success(true);
   }

   virtual void DeleteGoal(::google::protobuf::RpcController* controller,
                       const ::common::UniqueId* request,
                       ::common::OperationResultMessage* response,
                       ::google::protobuf::Closure* done)
   {
      mStrategy.deleteGoal(fromProto(*request));
      response->set_success(true);
   }

   virtual void GetGoals(::google::protobuf::RpcController* controller,
                       const ::common::EmptyMessage* request,
                       ::strategy::Goals* response,
                       ::google::protobuf::Closure* done)
   {
      for(auto x : mStrategy.getGoals())
      {
         response->add_items()->CopyFrom(toProto(x));
      }
   }

   virtual void GetGoal(::google::protobuf::RpcController* controller,
                       const ::common::UniqueId* request,
                       ::strategy::Goal* response,
                       ::google::protobuf::Closure* done)
   {
      auto g = mStrategy.getGoal(fromProto(*request));
      if(g)
      {
         response->CopyFrom(toProto(*g));
      }
   }

   virtual void AddObjective(::google::protobuf::RpcController* controller,
                       const ::strategy::Objective* request,
                       ::common::UniqueId* response,
                       ::google::protobuf::Closure* done)
   {
      response->CopyFrom(toProto(mStrategy.addObjective(fromProto(*request))));
   }

   virtual void ModifyObjective(::google::protobuf::RpcController* controller,
                       const ::strategy::Objective* request,
                       ::common::OperationResultMessage* response,
                       ::google::protobuf::Closure* done)
   {
      mStrategy.modifyObjective(fromProto(*request));
      response->set_success(true);
   }

   virtual void DeleteObjective(::google::protobuf::RpcController* controller,
                       const ::common::UniqueId* request,
                       ::common::OperationResultMessage* response,
                       ::google::protobuf::Closure* done)
   {
      mStrategy.deleteObjective(fromProto(*request));
      response->set_success(true);
   }

   virtual void AddTask(::google::protobuf::RpcController* controller,
                       const ::strategy::Task* request,
                       ::common::UniqueId* response,
                       ::google::protobuf::Closure* done)
   {
      response->CopyFrom(toProto(mStrategy.addTask(fromProto(*request))));
   }

   virtual void ModifyTask(::google::protobuf::RpcController* controller,
                       const ::strategy::Task* request,
                       ::common::OperationResultMessage* response,
                       ::google::protobuf::Closure* done)
   {
      mStrategy.modifyTask(fromProto(*request));
      response->set_success(true);
   }

   virtual void DeleteTask(::google::protobuf::RpcController* controller,
                       const ::common::UniqueId* request,
                       ::common::OperationResultMessage* response,
                       ::google::protobuf::Closure* done)
   {
      mStrategy.deleteTask(fromProto(*request));
      response->set_success(true);
   }

   virtual void GetGoalItems(::google::protobuf::RpcController* controller,
                       const ::common::UniqueId* request,
                       ::strategy::GoalItems* response,
                       ::google::protobuf::Closure* done)
   {
      auto [ts, os] = mStrategy.getGoalItems(fromProto(*request));
      for(auto x : os)
      {
         response->add_objectives()->CopyFrom(toProto(x));
      }
      for(auto x : ts)
      {
         response->add_tasks()->CopyFrom(toProto(x));
      }
   }

   virtual void AddResource(::google::protobuf::RpcController* controller,
                       const ::strategy::Resource* request,
                       ::common::UniqueId* response,
                       ::google::protobuf::Closure* done)
   {
      response->CopyFrom(toProto(mStrategy.addResource(fromProto(*request))));
   }

   virtual void ModifyResource(::google::protobuf::RpcController* controller,
                       const ::strategy::Resource* request,
                       ::common::OperationResultMessage* response,
                       ::google::protobuf::Closure* done)
   {
      mStrategy.modifyResource(fromProto(*request));
      response->set_success(true);
   }

   virtual void DeleteResource(::google::protobuf::RpcController* controller,
                       const ::common::UniqueId* request,
                       ::common::OperationResultMessage* response,
                       ::google::protobuf::Closure* done)
   {
      mStrategy.deleteResource(fromProto(*request));
      response->set_success(true);
   }

   virtual void GetResources(::google::protobuf::RpcController* controller,
                       const ::common::EmptyMessage* request,
                       ::strategy::Resources* response,
                       ::google::protobuf::Closure* done)
   {
      for(auto x : mStrategy.getResources())
      {
         response->add_items()->CopyFrom(toProto(x));
      }
   }

private:
   IStrategy& mStrategy;
};

}