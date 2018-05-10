#include "../Id.hpp"
#include "messages/common.pb.h"

namespace materia
{
   const Id Id::Invalid = Id("");

   Id::Id()
   {
      *this = Invalid;
   }

   Id::Id(const std::string& guid)
   : mGuid(guid)
   {

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

   bool Id::operator < (const Id& other) const
   {
      return mGuid < other.mGuid;
   }
}