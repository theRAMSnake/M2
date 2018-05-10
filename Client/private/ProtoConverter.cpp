#include "ProtoConverter.hpp"

namespace materia
{

common::UniqueId toProto(const Id& id)
{
   common::UniqueId result;

   result.set_guid(id.getGuid());

   return result;
}

Id fromProto(const common::UniqueId& protoId)
{
   return Id(protoId.guid());
}

}