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
        {"title", Type::String},
        {"modified", Type::Timestamp},
        }});

    result.push_back({"journal_content", "journal_content_new", {
        {"content", Type::String}
        }});
    result.back().handlers.onChanged = std::bind(&JournalSS::handleJournalContentItemChange, this, std::placeholders::_1);

    return result;
}

void JournalSS::updateHeaderModifiedTime(const Id& pageId)
{
    auto cons = mOm.getConnections().get(pageId);
    auto pos = std::find_if(cons.begin(), cons.end(), [&](auto x){return x.b == pageId && x.type == ConnectionType::Extension;});

    if(pos == cons.end())
    {
        throw std::runtime_error("Journal is corrupted, " + pageId.getGuid() + " is a page without header");
    }

    auto headerId = pos->a;
    auto header = mOm.get(headerId);

    (header)["modified"] = Time{std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())};

    mOm.modify(header);
}

void JournalSS::handleJournalContentItemChange(Object& obj)
{
    updateHeaderModifiedTime(obj.getId());
}

std::vector<CommandDef> JournalSS::getCommandDefs()
{
    return {};
}

}
