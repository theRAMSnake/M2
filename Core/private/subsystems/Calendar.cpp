#include "Calendar.hpp"
#include "../ObjectManager.hpp"

namespace materia
{

CalendarSS::CalendarSS(ObjectManager& objMan)
: mOm(objMan)
{

}

void CalendarSS::onNewDay()
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
        {"reccurencyType", Type::Option, {"None", "Weekly", "Monthly", "Quarterly", "Yearly", "Bi-daily"}},
        {"entityType", Type::Option, {"Event", "Task", "StrategyNodeReference"}},
        {"urgency", Type::Option, {"Not urgent", "Urgent"}}
        }});

    return result;
}

std::vector<CommandDef> CalendarSS::getCommandDefs()
{
    return {};
}

}