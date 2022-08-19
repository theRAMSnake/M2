
#pragma once

#include <variant>
#include "Object.hpp"
#include "Connections.hpp"

namespace materia
{

namespace v2
{

using Value = std::variant<bool, std::int64_t, double, std::string, Period, Money>;
class InterpreterContext
{
public:
    InterpreterContext(const Connections& cons);
    const Connections& getConnections() const;
    void setObject(const Object& o);
    const Object& getObject() const;

private:
    const Connections& mConns;
    const Object* mObject;
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
