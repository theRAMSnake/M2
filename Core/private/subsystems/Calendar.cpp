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
        {"reccurencyTypeChoice", Type::Choice, {"None", "Weekly", "Monthly", "Quarterly", "Yearly", "Bi-daily"}},
        {"entityTypeChoice", Type::Choice, {"Event", "Task", "StrategyNodeReference"}},
        {"urgencyChoice", Type::Choice, {"Not urgent", "Urgent"}}
        }});

    return result;
}

std::vector<CommandDef> CalendarSS::getCommandDefs()
{
    return {};
}

}