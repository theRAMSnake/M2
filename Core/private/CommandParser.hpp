#pragma once
#include "Commands.hpp"

#include <string>
#include <boost/property_tree/ptree.hpp>

namespace materia
{

std::unique_ptr<Command> parseCommand(const std::string& json);

}