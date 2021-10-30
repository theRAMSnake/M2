#pragma once

#include <variant>
#include "Object.hpp"

namespace materia
{

using Value = std::variant<std::int64_t, double, std::string, bool>;
class Connections;
class Expression
{
public:
    virtual Value evaluate(const Object& object, const Connections& cons) const = 0;

    virtual ~Expression(){}
};

std::shared_ptr<Expression> parseExpression(const std::string& src);

}
