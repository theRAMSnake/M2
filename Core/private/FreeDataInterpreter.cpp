#include "FreeDataInterpreter.hpp"
#include <boost/lexical_cast.hpp>
#include <regex>

namespace materia
{
namespace freedata
{

bool Interpreter::execBoolean(const std::string& expression) const
{
   return false;
}

bool is_number(const std::string& s)
{
   try 
   {
      int x = boost::lexical_cast<int>(s);
      return true;
   }
   catch(...) 
   { 
      return false;
   }
}

std::optional<BinaryExpression> Interpreter::compile(const std::string& expression) const
{
   std::optional<BinaryExpression> result;

   const std::regex reg("(-*\\w+)\\s(=|>|<)\\s(-*\\w+)");
   std::smatch m;

   if (std::regex_match(expression, m, reg) && m.size() == 4)
   {
      auto arg1 = m[1];
      auto sign = m[2];
      auto arg2 = m[3];

      result.reset(BinaryExpression(arg1, arg2, sign));
   }

   return result;
}

}
}