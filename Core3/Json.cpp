#include "Json.hpp"

namespace materia3
{

std::string Json::str() const
{
   std::ostringstream str;
   boost::property_tree::json_parser::write_json(str, mImpl);

   return str.str();
}

Json::Json()
{

}

Json::Json(const std::string& src)
{
   std::istringstream s(src);
   boost::property_tree::json_parser::read_json(s, mImpl);
}

bool Json::contains(const std::string& fname) const
{
   return mImpl.find(fname) != mImpl.not_found();
}

}