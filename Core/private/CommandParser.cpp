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
   auto typeName = getOrThrow<std::string>(src, "typename", "Typename is not specified");
   std::optional<Id> stdid;
   auto id = src.get_optional<std::string>("defined_id");
   if(id)
   {
       stdid = *id;
   }
   auto params = parseParams(src);

   return new CreateCommand(stdid, typeName, writeJson(params));
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

Command* parseQuery(const boost::property_tree::ptree& src)
{
   auto ids = parseIds(src);
   auto filter = parseFilter(src);

   return new QueryCommand(filter, ids);
}

Command* parseDestroy(const boost::property_tree::ptree& src)
{
   auto id = getOrThrow<Id>(src, "id", "Id is not specified");

   return new DestroyCommand(id);
}

Command* parseModify(const boost::property_tree::ptree& src)
{
   auto id = getOrThrow<Id>(src, "id", "Id is not specified");
   auto params = parseParams(src);

   return new ModifyCommand(id, writeJson(params));
}

Command* parseDescribe(const boost::property_tree::ptree& src)
{
   return new DescribeCommand();
}

Command* parseChangeType(const boost::property_tree::ptree& src)
{
   auto id = getOrThrow<Id>(src, "id", "Id is not specified");
   auto typeName = getOrThrow<std::string>(src, "typename", "Typename is not specified");
   return new ChangeTypeCommand(id, typeName);
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
    {"describe", parseDescribe},
    {"change_type", parseChangeType}
};

std::unique_ptr<Command> parseCommand(const std::string& json)
{
    boost::property_tree::ptree props = readJson<boost::property_tree::ptree>(json);

    auto opName = getOrThrow<std::string>(props, "operation", "Operation type is not specified");
    auto opParser = findOrThrow(gCommandParsers, opName, fmt::format("Operation {} unsupported", opName)).parseFunc;

    return std::unique_ptr<Command>(opParser(props));
}

}