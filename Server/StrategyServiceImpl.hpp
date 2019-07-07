#pragma once

#include <messages/strategy.pb.h>
#include <Core/IStrategy.hpp>
#include <Core/IStrategy_v2.hpp>
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

strategy::NodeType toProto(const materia::NodeType& src)
{
   switch (src)
   {
      case materia::NodeType::Blank: return strategy::NodeType::BLANK;
      case materia::NodeType::Counter: return strategy::NodeType::COUNTER;
      case materia::NodeType::Goal: return strategy::NodeType::GOAL;
      case materia::NodeType::Task: return strategy::NodeType::TASK;
      case materia::NodeType::Watch: return strategy::NodeType::WATCH;
      case materia::NodeType::Wait: return strategy::NodeType::WAIT;
      case materia::NodeType::Reference: return strategy::NodeType::REFERENCE;
   }
   
   return strategy::NodeType::BLANK;
}

materia::NodeType fromProto(const strategy::NodeType src)
{
   switch (src)
   {
      case strategy::NodeType::BLANK: return materia::NodeType::Blank;
      case strategy::NodeType::COUNTER: return materia::NodeType::Counter;
      case strategy::NodeType::GOAL: return materia::NodeType::Goal; 
      case strategy::NodeType::TASK: return materia::NodeType::Task;
      case strategy::NodeType::WATCH: return materia::NodeType::Watch;
      case strategy::NodeType::WAIT: return materia::NodeType::Wait;
      case strategy::NodeType::REFERENCE: return materia::NodeType::Reference;

   default:
      return materia::NodeType::Blank;
   }
}

class StrategyServiceImpl : public strategy::StrategyService
{
public:
   StrategyServiceImpl(ICore& core)
   : mStrategy(core.getStrategy())
   , mStrategy2(core.getStrategy_v2())
   {

   }

   virtual void AddGoal(::google::protobuf::RpcController* controller,
                       const ::strategy::Goal* request,
                       ::common::UniqueId* response,
                       ::google::protobuf::Closure* done)
   {
      response->CopyFrom(toProto(mStrategy2.addGoal(fromProto(*request))));
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
      mStrategy2.deleteGoal(fromProto(*request));
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

   virtual void GetGraph(::google::protobuf::RpcController* controller,
                       const ::common::UniqueId* request,
                       ::strategy::GraphDefinition* response,
                       ::google::protobuf::Closure* done)
   {
      auto g = mStrategy2.getGraph(fromProto(*request));
      if(g)
      {
         for(auto x : g->getLinks())
         {
            auto l = response->add_links();
            l->mutable_graphid()->CopyFrom(*request);
            l->mutable_from_node_id()->CopyFrom(toProto(x.from));
            l->mutable_to_node_id()->CopyFrom(toProto(x.to));
         }

         for(auto x : g->getNodes())
         {
            auto n = response->add_nodes();
            n->set_node_type(toProto(x.type));
            n->mutable_id()->mutable_graphid()->CopyFrom(*request);
            n->mutable_id()->mutable_objectid()->CopyFrom(toProto(x.id));

            auto& attrs = *n->mutable_attrs();
            auto srcAttrs = g->getNodeAttributes(x.id);
            if(srcAttrs.contains(NodeAttributeType::BRIEF))
            {
               attrs.set_brief(srcAttrs.get<NodeAttributeType::BRIEF>());
            }
            if(srcAttrs.contains(NodeAttributeType::IS_DONE))
            {
               attrs.set_done(srcAttrs.get<NodeAttributeType::IS_DONE>());
            }
            if(srcAttrs.contains(NodeAttributeType::PROGRESS_TOTAL))
            {
               attrs.set_progress_total(srcAttrs.get<NodeAttributeType::PROGRESS_TOTAL>());
            }
            if(srcAttrs.contains(NodeAttributeType::PROGRESS_CURRENT))
            {
               attrs.set_progress_current(srcAttrs.get<NodeAttributeType::PROGRESS_CURRENT>());
            }
            if(srcAttrs.contains(NodeAttributeType::WATCH_ITEM_REFERENCE))
            {
               attrs.mutable_watch_item_reference()->CopyFrom(toProto(srcAttrs.get<NodeAttributeType::WATCH_ITEM_REFERENCE>()));
            }
            if(srcAttrs.contains(NodeAttributeType::REQUIRED_TIMESTAMP))
            {
               attrs.set_required_timestamp(srcAttrs.get<NodeAttributeType::REQUIRED_TIMESTAMP>());
            }
            if(srcAttrs.contains(NodeAttributeType::GOAL_REFERENCE))
            {
               attrs.mutable_graph_reference()->CopyFrom(toProto(srcAttrs.get<NodeAttributeType::GOAL_REFERENCE>()));
            }
         }
      }
   }

   virtual void CreateLink(::google::protobuf::RpcController* controller,
                       const ::strategy::LinkProperties* request,
                       ::common::EmptyMessage* response,
                       ::google::protobuf::Closure* done)
   {
      mStrategy2.createLink(fromProto(request->graphid()), fromProto(request->from_node_id()), fromProto(request->to_node_id()));
   }

   virtual void CreateNode(::google::protobuf::RpcController* controller,
                       const ::strategy::NodeProperties* request,
                       ::common::UniqueId* response,
                       ::google::protobuf::Closure* done)
   {
      auto result = mStrategy2.createNode(fromProto(request->id().graphid()));
      response->CopyFrom(toProto(result));
   }

   virtual void ModifyNode(::google::protobuf::RpcController* controller,
                       const ::strategy::NodeProperties* request,
                       ::common::OperationResultMessage* response,
                       ::google::protobuf::Closure* done)
   {
      auto graphId = fromProto(request->id().graphid());
      auto nodeId = fromProto(request->id().objectid());

      materia::NodeAttributes attrs;
      auto& srcAttrs = request->attrs();
      if(!srcAttrs.brief().empty())
      {
         attrs.set<materia::NodeAttributeType::BRIEF>(srcAttrs.brief());
      }   

      attrs.set<materia::NodeAttributeType::IS_DONE>(srcAttrs.done());

      if(srcAttrs.progress_total() != 0)
      {
         attrs.set<materia::NodeAttributeType::PROGRESS_CURRENT>(srcAttrs.progress_current());
         attrs.set<materia::NodeAttributeType::PROGRESS_TOTAL>(srcAttrs.progress_total());
      }
      
      if(!srcAttrs.graph_reference().guid().empty())
      {
         attrs.set<materia::NodeAttributeType::GOAL_REFERENCE>(fromProto(srcAttrs.graph_reference()));
      }

      if(!srcAttrs.watch_item_reference().guid().empty())
      {
         attrs.set<materia::NodeAttributeType::WATCH_ITEM_REFERENCE>(fromProto(srcAttrs.watch_item_reference()));
      }
      
      if(srcAttrs.required_timestamp() != 0)
      {
         attrs.set<materia::NodeAttributeType::REQUIRED_TIMESTAMP>(srcAttrs.required_timestamp());
      }

      mStrategy2.setNodeAttributes(graphId, nodeId, fromProto(request->node_type()), attrs);
   }

   virtual void DeleteNode(::google::protobuf::RpcController* controller,
                       const ::strategy::GraphObjectId* request,
                       ::common::OperationResultMessage* response,
                       ::google::protobuf::Closure* done)
   {
      auto graphId = fromProto(request->graphid());
      auto nodeId = fromProto(request->objectid());

      mStrategy2.deleteNode(graphId, nodeId);
   }

   virtual void DeleteLink(::google::protobuf::RpcController* controller,
                       const ::strategy::LinkProperties* request,
                       ::common::OperationResultMessage* response,
                       ::google::protobuf::Closure* done)
   {
      mStrategy2.breakLink(fromProto(request->graphid()), fromProto(request->from_node_id()), fromProto(request->to_node_id()));
   }

   virtual void GetWatchItems(::google::protobuf::RpcController* controller,
                       const ::common::EmptyMessage* request,
                       ::strategy::WatchItems* response,
                       ::google::protobuf::Closure* done)
   {
      for(auto x : mStrategy2.getWatchItems())
      {
         auto item = response->add_items();
         *item->mutable_id() = toProto(x.id);
         item->set_text(x.text);
      }
   }

   virtual void DeleteWatchItem(::google::protobuf::RpcController* controller,
                        const ::common::UniqueId* request,
                        ::common::OperationResultMessage* response,
                        ::google::protobuf::Closure* done)
   {
      mStrategy2.removeWatchItem(fromProto(*request));
      response->set_success(true);
   }

   virtual void EditWatchItem(::google::protobuf::RpcController* controller,
                        const ::strategy::WatchItemInfo* request,
                        ::common::OperationResultMessage* response,
                        ::google::protobuf::Closure* done)
   {
      mStrategy2.replaceWatchItem({fromProto(request->id()), request->text()});
      response->set_success(true);
   }

   virtual void AddWatchItem(::google::protobuf::RpcController* controller,
                        const ::strategy::WatchItemInfo* request,
                        ::common::UniqueId* response,
                        ::google::protobuf::Closure* done)
   {
      *response = toProto(mStrategy2.addWatchItem({ materia::Id::Invalid, request->text() }));
   }

   virtual void GetFocusItems(::google::protobuf::RpcController* controller,
                       const ::common::EmptyMessage* request,
                       ::strategy::FocusItems* response,
                       ::google::protobuf::Closure* done)
   {
      for(auto x : mStrategy2.getFocusItems())
      {
         auto item = response->add_items();
         *item->mutable_details()->mutable_graphid() = toProto(x.graphId);
         *item->mutable_details()->mutable_objectid() = toProto(x.nodeId);
         *item->mutable_id() = toProto(x.id);
      }
   }

   virtual void DeleteFocusItem(::google::protobuf::RpcController* controller,
                        const ::common::UniqueId* request,
                        ::common::OperationResultMessage* response,
                        ::google::protobuf::Closure* done)
   {
      mStrategy2.removeFocusItem(fromProto(*request));
      response->set_success(true);
   }

   virtual void CompleteFocusItem(::google::protobuf::RpcController* controller,
                        const ::strategy::FocusItemInfo* request,
                        ::common::OperationResultMessage* response,
                        ::google::protobuf::Closure* done)
   {
      mStrategy2.completeFocusItem({fromProto(request->details().graphid()), fromProto(request->details().objectid()), fromProto(request->id())});
      response->set_success(true);
   }

   virtual void AddFocusItem(::google::protobuf::RpcController* controller,
                        const ::strategy::FocusItemInfo* request,
                        ::common::UniqueId* response,
                        ::google::protobuf::Closure* done)
   {
      *response = toProto(mStrategy2.addFocusItem({fromProto(request->details().graphid()), fromProto(request->details().objectid())}));
   }

private:
   IStrategy& mStrategy;
   IStrategy_v2& mStrategy2;
};

}