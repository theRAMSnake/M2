#pragma once

#include <variant>
#include "Object.hpp"

namespace materia
{

using Value = std::variant<int, double, std::string, bool>;
class Expression
{
public:
    virtual Value evaluate(const Object& object) const = 0;

    virtual ~Expression(){}
};

std::shared_ptr<Expression> parseExpression(const std::string& src);

}