#pragma once

#include <string>
#include <boost/property_tree/ptree.hpp>

namespace materia
{

class Command
{
public:
    virtual void execute(ObjectManager& objManager) = 0;
    virtual ~Command() {}
};

std::unique_ptr<Command> parseCommand(const std::string& json);

}