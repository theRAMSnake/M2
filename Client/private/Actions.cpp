#include "Actions.hpp"
#include "ProtoConverter.hpp"

namespace materia
{

Actions::Actions(materia::ZmqPbChannel& channel)
: mProxy(channel)
{

}

ActionType fromProto(const actions::ActionType t)
{
   switch(t)
   {
      case actions::Task:
         return ActionType::Task;

      case actions::Group:
         return ActionType::Group;

      default:
         return ActionType::Unknown;
   }
}

materia::ActionItem fromProto(const actions::ActionInfo& x)
{
   return { fromProto(x.id()), fromProto(x.parentid()), x.title(), x.description(), fromProto(x.type())};
}

std::vector<ActionItem> Actions::getRootItems()
{
   common::EmptyMessage emptyMsg;
   actions::ActionsList responce;

   mProxy.getService().GetParentlessElements(nullptr, &emptyMsg, &responce, nullptr);

   std::vector<ActionItem> result(responce.list_size());
   std::transform(responce.list().begin(), responce.list().end(), result.begin(), [] (auto x)-> auto { return fromProto(x); });

   return result;
}

std::vector<ActionItem> Actions::getChildren(const Id& id)
{
   actions::ActionsList responce;
   auto protoId = toProto(id);
   mProxy.getService().GetChildren(nullptr, &protoId, &responce, nullptr);

   std::vector<ActionItem> result(responce.list_size());
   std::transform(responce.list().begin(), responce.list().end(), result.begin(), [] (auto x)-> auto { return fromProto(x); });

   return result;
}

bool Actions::deleteItem(const Id& id)
{
   auto protoId = toProto(id);
   
   common::OperationResultMessage opResult;
   mProxy.getService().DeleteElement(nullptr, &protoId, &opResult, nullptr);
   
   return opResult.success();
}

actions::ActionType toProto(const ActionType t)
{
   switch(t)
   {
      case ActionType::Task:
         return actions::Task;

      case ActionType::Group:
         return actions::Group;

      default:
         throw -1;
   }
}

actions::ActionInfo fillRequest(const ActionItem& item)
{
   actions::ActionInfo request;
   
   request.set_title(item.title);
   request.set_description(item.description);
   request.set_type(toProto(item.type));
   request.mutable_id()->CopyFrom(toProto(item.id));
   request.mutable_parentid()->CopyFrom(toProto(item.parentId));

   return request;
}

bool Actions::replaceItem(const ActionItem& item)
{
   actions::ActionInfo request = fillRequest(item);
   
   common::OperationResultMessage opResult;
   mProxy.getService().EditElement(nullptr, &request, &opResult, nullptr);
   
   return opResult.success();
}

Id Actions::insertItem(const ActionItem& item)
{
   common::UniqueId id;

   actions::ActionInfo request = fillRequest(item);

   mProxy.getService().AddElement(nullptr, &request, &id, nullptr);
   
   return fromProto(id);
}

bool ActionItem::operator == (const ActionItem& other) const
{
   return id == other.id && parentId == other.parentId && title == other.title && description == other.description && type == other.type;
}

bool ActionItem::operator != (const ActionItem& other) const
{
   return !operator==(other);
}

}