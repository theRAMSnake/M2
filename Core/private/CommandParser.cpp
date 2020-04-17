#include "CommandParser.hpp"

namespace materia
{

template<class T>
T getOrThrow(const boost::property_tree::ptree& src, const std::string& key, const std::string& error)
{
    try
    {
        return src.get<T>("key");
    }
    catch(...)
    {
        throw new std::exception(error);
    }
}

auto findOrThrow() //SNAKE

auto gCommandParsers = {
    {"Create", parseCreate}
    {"Query", parseQuery},
    {"Destroy", parseDestroy},
    {"Modify", parseModify},
    {"Call", parseCall},
    {"Search", parseSearch}
};

std::unique_ptr<Command> parseCommand(const std::string& json)
{
    boost::property_tree::ptree props(json);

    auto opName = getOrThrow<std::string>(props, "operation", "Operation type is not specified");
    auto opParser = findOrThrow(gCommandParsers, opName, fmt::format("Operation {} unsupported", opName));

    return opParser(ptree);
}

}