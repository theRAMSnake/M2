#include "FreeDataInterpreter.hpp"
#include <boost/lexical_cast.hpp>
#include <regex>

namespace materia
{
namespace freedata
{

bool Interpreter::execBoolean(const BinaryExpression& expression) const
{
   int arg1 = boost::lexical_cast<int>(expression.arg1);
   int arg2 = boost::lexical_cast<int>(expression.arg2);

   switch(expression.sign)
   {
      case '=':
         return arg1 == arg2;

      case '>':
         return arg1 > arg2;

      case '<':
         return arg1 < arg2;
   }

   return false;
}

std::optional<BinaryExpression> Interpreter::compile(const std::string& expression) const
{
   std::optional<BinaryExpression> result;

   const std::regex reg("(-*\\w+)\\s(=|>|<)\\s(-*\\w+)");
   std::smatch m;

   if (std::regex_match(expression, m, reg) && m.size() == 4)
   {
      auto arg1 = m[1].str();
      auto sign = m[2].str();
      auto arg2 = m[3].str();

      result = BinaryExpression{arg1, arg2, sign[0]};
   }

   return result;
}

}
}