#pragma once
#include <string>
#include <optional>

namespace materia
{
namespace freedata
{

struct BinaryExpression
{
   std::string arg1;
   std::string arg2;
   char sign;
};

class Interpreter
{
public:
   bool execBoolean(const BinaryExpression& expression) const;

   std::optional<BinaryExpression> compile(const std::string& expression) const;
};

}
}