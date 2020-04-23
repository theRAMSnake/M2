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

using ParseFunc = Command*(*)(const boost::property_tree::ptree&);

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

Command* parseCreate(const boost::property_tree::ptree& src)
{
   auto tpDomain = getOrThrow<std::string>(src, "type.domain", "Type domain is not specified");
   auto tpName = getOrThrow<std::string>(src, "type.name", "Type name is not specified");
   auto params = parseParams(src);

   return new CreateCommand({tpDomain, tpName}, params);
}

Command* parseQuery(const boost::property_tree::ptree& src)
{
   auto tpDomain = getOrThrow<std::string>(src, "type.domain", "Type domain is not specified");
   auto tpName = getOrThrow<std::string>(src, "type.name", "Type name is not specified");
   auto filter = parseFilter(src);

   return new QueryCommand({tpDomain, tpName}, filter, src.get_optional<std::string>("id"));
}

Command* parseDestroy(const boost::property_tree::ptree& src)
{
   auto tpDomain = getOrThrow<std::string>(src, "type.domain", "Type domain is not specified");
   auto tpName = getOrThrow<std::string>(src, "type.name", "Type name is not specified");
   auto id = getOrThrow<Id>(src, "id", "Id is not specified");

   return new DestroyCommand({tpDomain, tpName}, id);
}

Command* parseModify(const boost::property_tree::ptree& src)
{
   auto tpDomain = getOrThrow<std::string>(src, "type.domain", "Type domain is not specified");
   auto tpName = getOrThrow<std::string>(src, "type.name", "Type name is not specified");
   auto params = parseParams(src);

   return new ModifyCommand({tpDomain, tpName}, params);
}

Command* parseCall(const boost::property_tree::ptree& src)
{
   auto opName = getOrThrow<std::string>(src, "script", "Script is not specified");
   auto params = parseParams(src);

   return new CallCommand(opName, params);
}

Command* parseSearch(const boost::property_tree::ptree& src)
{
   auto phrase = getOrThrow<std::string>(src, "phrase", "Phrase is not specified");

   return new SearchCommand(phrase);
}

struct CommandDef
{
    std::string name;
    ParseFunc parseFunc;
};

std::vector<CommandDef> gCommandParsers = {
    {"create", parseCreate},
    {"query", parseQuery},
    {"destroy", parseDestroy},
    {"modify", parseModify},
    {"call", parseCall},
    {"search", parseSearch}
};

std::unique_ptr<Command> parseCommand(const std::string& json)
{
    boost::property_tree::ptree props = readJson<boost::property_tree::ptree>(json);

    auto opName = getOrThrow<std::string>(props, "operation", "Operation type is not specified");
    auto opParser = findOrThrow(gCommandParsers, opName, fmt::format("Operation {} unsupported", opName)).parseFunc;

    return std::unique_ptr<Command>(opParser(props));
}

}