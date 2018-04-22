#include "Strategy.hpp"

namespace materia
{

Strategy::Strategy(materia::ZmqPbChannel& channel)
: mProxy(channel)
{

}

strategy::CommonItemProperties toProto(const materia::StrategyItem& x)
{
   strategy::CommonItemProperties result;

   result.mutable_id()->CopyFrom(x.id.toProtoId());
   result.mutable_parent_goal_id()->CopyFrom(x.parentGoalId.toProtoId());

   for(auto a : x.requirementsIds)
   {
      result.add_requrements_item_id()->CopyFrom(a.toProtoId());
   }

   result.set_name(x.name);
   result.set_notes(x.notes);
   result.mutable_icon_id()->CopyFrom(x.iconId.toProtoId());

   return result;
}

template<class T>
T fromProto(const strategy::CommonItemProperties& x)
{
   T result;

   result.id = x.id();
   result.parentGoalId = x.parent_goal_id();

   for(auto a : x.requrements_item_id())
   {
      result.requirementsIds.push_back(a);
   }

   result.name = x.name();
   result.notes = x.notes();
   result.iconId = x.icon_id();

   return result;
}

strategy::Goal toProto(const materia::Goal& x)
{
   strategy::Goal result;

   result.mutable_common_props()->CopyFrom(toProto(static_cast<const materia::StrategyItem&>(x)));
   result.mutable_affinityid()->CopyFrom(x.affinityId.toProtoId());
   result.set_focused(x.focused);
   result.set_achieved(x.achieved);

   return result;
}

materia::Goal fromProto(const strategy::Goal& x)
{
   materia::Goal result = fromProto<Goal>(x.common_props());

   result.affinityId = x.affinityid();
   result.focused = x.focused();
   result.achieved = x.achieved();

   return result;
}

strategy::Task toProto(const materia::Task& x)
{
   strategy::Task result;

   result.mutable_common_props()->CopyFrom(toProto(static_cast<const materia::StrategyItem&>(x)));
   result.mutable_action_ref()->CopyFrom(x.actionReference.toProtoId());
   result.mutable_calendar_ref()->CopyFrom(x.calendarReference.toProtoId());
   result.set_done(x.done);
   result.set_count(x.count);

   return result;
}

materia::Task fromProto(const strategy::Task& x)
{
   materia::Task result = fromProto<Task>(x.common_props());

   result.done = x.done();
   result.count = x.count();
   result.actionReference = x.action_ref();
   result.calendarReference = x.calendar_ref();

   return result;
}

strategy::Objective toProto(const materia::Objective& x)
{
   strategy::Objective result;

   result.mutable_common_props()->CopyFrom(toProto(static_cast<const materia::StrategyItem&>(x)));
   result.mutable_meas_id()->CopyFrom(x.measurementId.toProtoId());
   result.set_reached(x.reached);

   return result;
}

materia::Objective fromProto(const strategy::Objective& x)
{
   materia::Objective result = fromProto<Objective>(x.common_props());

   result.measurementId = x.meas_id();
   result.reached = x.reached();

   return result;
}

container::FuncType toProto(const FuncType src)
{
   switch(src)
   {
      case FuncType::Sum:
         return container::Sum;

      case FuncType::Count:
         return container::Count;

      default:
         throw -1;
   }
}

strategy::Measurement toProto(const materia::Measurement& x)
{
   strategy::Measurement result;

   result.mutable_id()->CopyFrom(x.id.toProtoId());
   result.set_value(x.value);
   result.set_name(x.name);
   result.set_of(x.of);
   result.mutable_icon_id()->CopyFrom(x.iconId.toProtoId());
   result.mutable_func()->set_func_type(toProto(x.func.funcType));
   result.mutable_func()->set_container_name(x.func.containerName);

   return result;
}

FuncType fromProto(const container::FuncType src)
{
   switch(src)
   {
      case container::FuncType::Sum:
         return FuncType::Sum;

      case container::FuncType::Count:
         return FuncType::Count;

      default:
         throw -1;
   }
}

materia::Measurement fromProto(const strategy::Measurement& x)
{
   materia::Measurement result;

   result.id = x.id();
   result.name = x.name();
   result.value = x.value();
   result.of = x.value();
   result.iconId = x.icon_id();
   result.func.funcType = fromProto(x.func().func_type());
   result.func.containerName = x.func().container_name();

   return result;
}

strategy::Affinity toProto(const materia::Affinity& x)
{
   strategy::Affinity result;

   result.mutable_id()->CopyFrom(x.id.toProtoId());
   result.set_name(x.name);
   result.set_colorname(x.colorName);
   result.mutable_icon_id()->CopyFrom(x.iconId.toProtoId());

   return result;
}

materia::Affinity fromProto(const strategy::Affinity& x)
{
   materia::Affinity result;

   result.id = x.id();
   result.name = x.name();
   result.colorName = x.colorname();
   result.iconId = x.icon_id();

   return result;
}

Id Strategy::addGoal(const Goal& goal)
{
   common::UniqueId id;
   auto proto = toProto(goal);

   mProxy.getService().AddGoal(nullptr, &proto, &id, nullptr);
   
   return Id(id);
}

bool Strategy::modifyGoal(const Goal& goal)
{
   auto request = toProto(goal);
   
   common::OperationResultMessage opResult;
   mProxy.getService().ModifyGoal(nullptr, &request, &opResult, nullptr);
   
   return opResult.success();
}

bool Strategy::deleteGoal(const Id& id)
{
   auto protoId = id.toProtoId();
   
   common::OperationResultMessage opResult;
   mProxy.getService().DeleteGoal(nullptr, &protoId, &opResult, nullptr);
   
   return opResult.success();
}

std::vector<Goal> Strategy::getGoals()
{
   common::EmptyMessage emptyMsg;
   strategy::Goals responce;

   mProxy.getService().GetGoals(nullptr, &emptyMsg, &responce, nullptr);

   std::vector<Goal> result(responce.items_size());
   std::transform(responce.items().begin(), responce.items().end(), result.begin(), [] (auto x)-> auto { return fromProto(x); });

   return result;
}

std::tuple<std::vector<Task>, std::vector<Objective>> Strategy::getGoalItems(const Id& id)
{
   auto protoId = id.toProtoId();
   strategy::GoalItems responce;

   mProxy.getService().GetGoalItems(nullptr, &protoId, &responce, nullptr);

   std::vector<Task> tasks(responce.tasks_size());
   std::transform(responce.tasks().begin(), responce.tasks().end(), tasks.begin(), [] (auto x)-> auto { return fromProto(x); });

   std::vector<Objective> objectives(responce.objectives_size());
   std::transform(responce.objectives().begin(), responce.objectives().end(), objectives.begin(), [] (auto x)-> auto { return fromProto(x); });

   return std::tuple<std::vector<Task>, std::vector<Objective>>(tasks, objectives);
}

Id Strategy::addObjective(const Objective& obj)
{
   common::UniqueId id;
   auto proto = toProto(obj);

   mProxy.getService().AddObjective(nullptr, &proto, &id, nullptr);
   
   return Id(id);
}

bool Strategy::modifyObjective(const Objective& obj)
{
   auto request = toProto(obj);
   
   common::OperationResultMessage opResult;
   mProxy.getService().ModifyObjective(nullptr, &request, &opResult, nullptr);
   
   return opResult.success();
}

bool Strategy::deleteObjective(const Id& id)
{
   auto protoId = id.toProtoId();
   
   common::OperationResultMessage opResult;
   mProxy.getService().DeleteObjective(nullptr, &protoId, &opResult, nullptr);
   
   return opResult.success();
}

Id Strategy::addTask(const Task& task)
{
   common::UniqueId id;
   auto proto = toProto(task);

   mProxy.getService().AddTask(nullptr, &proto, &id, nullptr);
   
   return Id(id);
}

bool Strategy::modifyTask(const Task& task)
{
   auto request = toProto(task);
   
   common::OperationResultMessage opResult;
   mProxy.getService().ModifyTask(nullptr, &request, &opResult, nullptr);
   
   return opResult.success();
}

bool Strategy::deleteTask(const Id& id)
{
   auto protoId = id.toProtoId();
   
   common::OperationResultMessage opResult;
   mProxy.getService().DeleteTask(nullptr, &protoId, &opResult, nullptr);
   
   return opResult.success();
}

Id Strategy::addMeasurement(const Measurement& meas)
{
   common::UniqueId id;
   auto proto = toProto(meas);

   mProxy.getService().AddMeasurement(nullptr, &proto, &id, nullptr);
   
   return Id(id);
}

bool Strategy::modifyMeasurement(const Measurement& meas)
{
   auto request = toProto(meas);
   
   common::OperationResultMessage opResult;
   mProxy.getService().ModifyMeasurement(nullptr, &request, &opResult, nullptr);
   
   return opResult.success();
}

bool Strategy::deleteMeasurement(const Id& id)
{
   auto protoId = id.toProtoId();
   
   common::OperationResultMessage opResult;
   mProxy.getService().DeleteMeasurement(nullptr, &protoId, &opResult, nullptr);
   
   return opResult.success();
}

Measurement Strategy::getMeasurement(const Id& id)
{
   common::UniqueId r = id.toProtoId();
   strategy::Measurement result;

   mProxy.getService().GetMeasurement(nullptr, &r, &result, nullptr);

   return fromProto(result);
}

Id Strategy::addAffinity(const Affinity& aff)
{
   common::UniqueId id;
   auto proto = toProto(aff);

   mProxy.getService().AddAffinity(nullptr, &proto, &id, nullptr);
   
   return Id(id);
}

bool Strategy::modifyAffinity(const Affinity& aff)
{
   auto request = toProto(aff);
   
   common::OperationResultMessage opResult;
   mProxy.getService().ModifyAffinity(nullptr, &request, &opResult, nullptr);
   
   return opResult.success();
}

bool Strategy::deleteAffinity(const Id& id)
{
   auto protoId = id.toProtoId();
   
   common::OperationResultMessage opResult;
   mProxy.getService().DeleteAffinity(nullptr, &protoId, &opResult, nullptr);
   
   return opResult.success();
}

std::vector<Affinity> Strategy::getAffinities()
{
   common::EmptyMessage r;
   strategy::Affinities responce;

   mProxy.getService().GetAffinities(nullptr, &r, &responce, nullptr);

   std::vector<Affinity> result(responce.items_size());
   std::transform(responce.items().begin(), responce.items().end(), result.begin(), [] (auto x)-> auto { return fromProto(x); });

   return result;
}

}