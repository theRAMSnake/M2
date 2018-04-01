#include "TestHelpers.hpp"

namespace std
{

std::ostream& operator << (std::ostream& str, const materia::Id& id)
{
   str << (id != materia::Id::Invalid ? id.getGuid() : "invalid_id");
   return str;
}

}
