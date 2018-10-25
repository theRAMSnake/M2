#include "Id.hpp"
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

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

   Id::operator std::string() const
   {
      return mGuid;
   }

   Id Id::generate()
   {
      static boost::uuids::random_generator generator;
      return to_string(generator());
   }
}