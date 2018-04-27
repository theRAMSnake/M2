#include "Container.hpp"

namespace materia
{

Container::Container(materia::ZmqPbChannel& channel)
: mProxy(channel)
{

}

bool Container::addContainer(const ContainerDefinition& def)
{
   container::Container c;
   c.set_name(def.name);
   c.set_is_public(def.isPublic);
   if(def.iconId != materia::Id::Invalid)
   {
      c.mutable_icon_id()->CopyFrom(def.iconId.toProtoId());
   }

   common::OperationResultMessage result;
   mProxy.getService().AddContainer(nullptr, &c, &result, nullptr);

   return result.success();
}

materia::ContainerDefinition fromProto(const container::Container& x)
{
   return { x.name(), x.is_public(), x.icon_id()};
}

std::vector<ContainerDefinition> Container::getPublicContainers()
{
   common::EmptyMessage in;

   container::Containers out;
   mProxy.getService().GetPublicContainers(nullptr, &in, &out, nullptr);

   std::vector<ContainerDefinition> result(out.containers_size());
   std::transform(out.containers().begin(), out.containers().end(), result.begin(), [] (auto x)-> auto { return fromProto(x); });

   return result;
}

bool Container::deleteContainer(const std::string& name)
{
   common::StringMessage s;
   s.set_content(name);
   
   common::OperationResultMessage opResult;
   mProxy.getService().DeleteContainer(nullptr, &s, &opResult, nullptr);
   
   return opResult.success();
}

bool Container::clearContainer(const std::string& name)
{
   common::StringMessage s;
   s.set_content(name);
   
   common::OperationResultMessage opResult;
   mProxy.getService().ClearContainer(nullptr, &s, &opResult, nullptr);
   
   return opResult.success();
}

materia::ContainerItem fromProto(const container::Item& x)
{
   materia::ContainerItem result;

   result.id = materia::Id(x.id());
   result.content = x.content();

   if(x.blob().size() > 0)
   {
      result.blob.resize(x.blob().size());
      memcpy(&result.blob.front(), &x.blob().front(), x.blob().size());
   }

   return result;
}

std::vector<ContainerItem> Container::getItems(const std::string& containerName)
{
   common::StringMessage s;
   s.set_content(containerName);

   container::Items out;
   mProxy.getService().GetItems(nullptr, &s, &out, nullptr);

   std::vector<ContainerItem> result(out.items_size());
   std::transform(out.items().begin(), out.items().end(), result.begin(), [] (auto x)-> auto { return fromProto(x); });

   return result;
}

container::Item toProto(const materia::ContainerItem& x)
{
   container::Item result;

   result.mutable_id()->CopyFrom(x.id.toProtoId());
   result.set_content(x.content);
   result.mutable_blob()->resize(x.blob.size());
   memcpy(&result.mutable_blob()->front(), &x.blob.front(), x.blob.size());

   return result;
}

namespace
{

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

}

boost::optional<int> Container::execFunc(const Func& func)
{
   container::Func f;
   f.set_func_type(toProto(func.funcType));
   f.set_container_name(func.containerName);

   container::FuncResult r;
   mProxy.getService().ExecFunc(nullptr, &f, &r, nullptr);

   if(r.op_result().success())
   {
      return r.value();
   }

   return boost::optional<int>();
}

std::vector<Id> Container::insertItems(const std::string& containerName, const std::vector<ContainerItem>& items)
{
   container::ItemsOfContainer in;
   in.set_container_name(containerName);

   std::for_each(items.begin(), items.end(), [&] (auto x)->auto { in.mutable_items()->add_items()->CopyFrom(toProto(x)); });

   common::IdSet result;
   mProxy.getService().InsertItems(nullptr, &in, &result, nullptr);

   std::vector<Id> ids(result.idset_size());
   std::transform(result.idset().begin(), result.idset().end(), ids.begin(), [] (auto x)-> auto { return materia::Id(x); });
   
   return ids;
}

bool Container::replaceItems(const std::string& containerName, const std::vector<ContainerItem>& items)
{
   container::ItemsOfContainer in;
   in.set_container_name(containerName);

   std::for_each(items.begin(), items.end(), [&] (auto x)->auto { in.mutable_items()->add_items()->CopyFrom(toProto(x)); });

   common::OperationResultMessage opResult;
   mProxy.getService().ReplaceItems(nullptr, &in, &opResult, nullptr);
   
   return opResult.success();
}

bool Container::deleteItems(const std::string& containerName, const std::vector<Id>& ids)
{
   container::DeleteItemsRequest in;
   in.set_container_name(containerName);

   std::for_each(ids.begin(), ids.end(), [&] (auto x)->auto { in.mutable_id_set()->add_idset()->CopyFrom(x.toProtoId()); });

   common::OperationResultMessage opResult;
   mProxy.getService().DeleteItems(nullptr, &in, &opResult, nullptr);
   
   return opResult.success();
}

std::vector<char> Container::fetch()
{
   common::EmptyMessage in;

   container::Backup out;
   mProxy.getService().Fetch(nullptr, &in, &out, nullptr);

   std::vector<char> result(out.blob().size());
   memcpy(&result.front(), &out.blob().front(), out.blob().size());

   return result;
}

bool ContainerDefinition::operator == (const ContainerDefinition& other) const
{
   return name == other.name;
}

bool ContainerItem::operator == (const ContainerItem& other) const
{
   return id == other.id && content == other.content && blob == other.blob;
}

bool ContainerItem::operator != (const ContainerItem& other) const
{
   return !operator==(other);
}

}