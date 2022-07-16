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

namespace v2
{

using Value = std::variant<bool>;
struct InterpreterContext
{
    /*InterpreterContext(const Object& object_)
        : object(object_)
    {
    }
    const Object& object;*/
};

class Expression
{
public:
    virtual Value evaluate(const InterpreterContext& context) const = 0;

    virtual ~Expression(){}
};

std::shared_ptr<Expression> parseExpression(const std::string& src);

}

}
