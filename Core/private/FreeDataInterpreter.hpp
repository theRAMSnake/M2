#pragma once
#include <string>
#include <optional>

namespace materia
{
namespace freedata
{

class BinaryExpression
{
public:

};

class Interpreter
{
public:
   bool execBoolean(const std::string& expression) const;

   std::optional<BinaryExpression> compile(const std::string& expression) const;
};

}
}