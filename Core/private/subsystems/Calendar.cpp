#include "Calendar.hpp"
#include "../ObjectManager.hpp"

namespace materia
{

CalendarSS::CalendarSS(ObjectManager& objMan)
: mOm(objMan)
{

}

void CalendarSS::onNewDay(const boost::gregorian::date& date)
{

}

void CalendarSS::onNewWeek()
{
   
}

std::vector<TypeDef> CalendarSS::getTypes()
{
    std::vector<TypeDef> result;

    result.push_back({"calendar_item", "calendar", {
        {"text", Type::String},
        {"timestamp", Type::Timestamp},
        {"recurrency", Type::Period},
        {"isShared", Type::Bool},
        {"entityTypeChoice", Type::Choice, {"Event", "Task", "StrategyNodeReference"}},
        {"urgencyChoice", Type::Choice, {"Not urgent", "Urgent"}}},
        {},
        {{"core_value", ConnectionType::Reference, "Core Value"}}
        });

    return result;
}

std::vector<CommandDef> CalendarSS::getCommandDefs()
{
    return {};
}

}
