#pragma once

#include <variant>
#include <boost/property_tree/ptree.hpp>

namespace materia
{

using Params = boost::property_tree::ptree;

using Value = std::variant<int, double, std::string, bool>;
class Expression
{
public:
    virtual Value evaluate(const Params& object) const = 0;

    virtual ~Expression(){}
};

std::shared_ptr<Expression> parseExpression(const std::string& src);

}