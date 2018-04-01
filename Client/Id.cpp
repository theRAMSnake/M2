#include "Id.hpp"

namespace materia
{
   const Id Id::Invalid = Id("");

   Id::Id()
   {
      *this = Invalid;
   }

   Id::Id(const common::UniqueId& protoId)
   : mGuid(protoId.guid())
   {
      
   }

   Id::Id(const std::string& guid)
   : mGuid(guid)
   {

   }

   common::UniqueId Id::toProtoId() const
   {
      common::UniqueId result;

      result.set_guid(mGuid);

      return result;
   }

   bool Id::operator == (const Id& other) const
   {
      return mGuid == other.mGuid;
   }

   bool Id::operator != (const Id& other) const
   {
      return !operator==(other);
   }
   
   const std::string& Id::getGuid() const
   {
      return mGuid;
   }
}