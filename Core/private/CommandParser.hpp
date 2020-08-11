#pragma once
#include "Commands.hpp"
#include <functional>
#include <string>
#include <boost/property_tree/ptree.hpp>

namespace materia
{

using ParseFunc = std::function<Command*(const boost::property_tree::ptree&)>;
struct CommandDef
{
    std::string name;
    ParseFunc parseFunc;
};

std::unique_ptr<Command> parseCommand(const std::string& json, const std::vector<CommandDef>& commandDefs);

boost::property_tree::ptree parseParams(const boost::property_tree::ptree& src);
std::vector<Id> parseIds(const boost::property_tree::ptree& src);
std::shared_ptr<Filter> parseFilter(const boost::property_tree::ptree& src);

}