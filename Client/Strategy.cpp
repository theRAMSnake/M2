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

   for(auto a : x.requiredGoals)
   {
      result.add_required_goals()->CopyFrom(a.toProtoId());
   }

   return result;
}

materia::Goal fromProto(const strategy::Goal& x)
{
   materia::Goal result = fromProto<Goal>(x.common_props());

   result.affinityId = x.affinityid();
   result.focused = x.focused();
   result.achieved = x.achieved();

   for(auto a : x.required_goals())
   {
      result.requiredGoals.push_back(a);
   }

   return result;
}

strategy::Task toProto(const materia::Task& x)
{
   strategy::Task result;

   result.mutable_common_props()->CopyFrom(toProto(static_cast<const materia::StrategyItem&>(x)));
   result.set_done(x.done);
   for(auto a : x.requiredTasks)
   {
      result.add_required_tasks()->CopyFrom(a.toProtoId());
   }

   return result;
}

materia::Task fromProto(const strategy::Task& x)
{
   materia::Task result = fromProto<Task>(x.common_props());

   result.done = x.done();
   for(auto a : x.required_tasks())
   {
      result.requiredTasks.push_back(a);
   }

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
   result.mutable_icon_id()->CopyFrom(x.iconId.toProtoId());

   return result;
}

materia::Measurement fromProto(const strategy::Measurement& x)
{
   materia::Measurement result;

   result.id = x.id();
   result.name = x.name();
   result.value = x.value();
   result.iconId = x.icon_id();

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

std::vector<Measurement> Strategy::getMeasurements()
{
   common::EmptyMessage r;
   strategy::Measurements responce;

   mProxy.getService().GetMeasurements(nullptr, &r, &responce, nullptr);

   std::vector<Measurement> result(responce.items_size());
   std::transform(responce.items().begin(), responce.items().end(), result.begin(), [] (auto x)-> auto { return fromProto(x); });

   return result;
}

void Strategy::configureAffinities(const std::vector<Affinity>& affinities)
{
   strategy::Affinities req;
   common::OperationResultMessage opResult;

   for(auto x : affinities)
   {
      req.add_items()->CopyFrom(toProto(x));
   }

   mProxy.getService().ConfigureAffinities(nullptr, &req, &opResult, nullptr);
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

bool Goal::operator == (const Goal& other) const
{
   return id == other.id
      && parentGoalId == other.parentGoalId
      && name == other.name
      && notes == other.notes
      && requiredGoals == other.requiredGoals
      && affinityId == other.affinityId
      && achieved == other.achieved
      && focused == other.focused;
}

}