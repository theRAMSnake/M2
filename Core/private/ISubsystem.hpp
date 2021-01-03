#pragma once
#include <vector>
#include "TypeSystem.hpp"
#include "CommandParser.hpp"
#include "boost/date_time/gregorian/gregorian_types.hpp"

namespace materia
{

class ISubsystem
{
public:
    virtual void onNewDay(const boost::gregorian::date& date) = 0;
    virtual void onNewWeek() = 0;
    virtual std::vector<TypeDef> getTypes() = 0;
    virtual std::vector<CommandDef> getCommandDefs() = 0;

    virtual ~ISubsystem(){}
};

}