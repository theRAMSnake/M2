#include "Journal.hpp"
#include "../ObjectManager.hpp"

namespace materia
{

JournalSS::JournalSS(ObjectManager& objMan)
: mOm(objMan)
{

}

void JournalSS::onNewDay(const boost::gregorian::date& date)
{

}

void JournalSS::onNewWeek()
{
   
}

std::vector<TypeDef> JournalSS::getTypes()
{
    std::vector<TypeDef> result;

    result.push_back({"journal_header", "journal_index", {
        {"title", Type::String}
        }});

    result.push_back({"journal_content", "journal_content_new", {
        {"content", Type::String}
        }});

    return result;
}


std::vector<CommandDef> JournalSS::getCommandDefs()
{
    return {};
}

}
