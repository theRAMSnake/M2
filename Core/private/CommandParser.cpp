#include "CommandParser.hpp"
#include "ExceptionsUtil.hpp"
#include "JsonSerializer.hpp"
#include <fmt/format.h>

namespace materia
{

template<class T>
T findOrThrow(const std::vector<T>& vec, const std::string& name, const std::string errMsg)
{
    auto pos = std::find_if(vec.begin(), vec.end(), [&](auto x){return x.name == name;}); 
    if(pos == vec.end())
    {
        throw std::runtime_error(fmt::format("Unknown name {}", name));
    }
    else
    {
        return *pos;
    } 
}

boost::property_tree::ptree parseParams(const boost::property_tree::ptree& src)
{
    try
    {
        return src.get_child("params");
    }
    catch(...)
    {
        throw std::runtime_error("Unable to get params");
    }
}

std::shared_ptr<Filter> parseFilter(const boost::property_tree::ptree& src)
{
    auto filter = src.get_optional<std::string>("filter");
    if(filter)
    {
        return parseExpression(*filter);
    }
    else
    {
        return std::shared_ptr<Filter>();
    }
}

std::vector<Id> parseIds(const boost::property_tree::ptree& src)
{
    std::vector<Id> result;
    auto val = src.get_child_optional("ids");
    if(val)
    {
        for(auto x : *val)
        {
            result.push_back(Id(x.second.get_value<std::string>()));
        }
    }

    return result;
}

std::unique_ptr<Command> parseCommand(const std::string& json, const std::vector<CommandDef>& commandDefs)
{
    boost::property_tree::ptree props = readJson<boost::property_tree::ptree>(json);

    auto opName = getOrThrow<std::string>(props, "operation", "Operation type is not specified");
    auto opParser = findOrThrow(commandDefs, opName, fmt::format("Operation {} unsupported", opName)).parseFunc;

    return std::unique_ptr<Command>(opParser(props));
}

}