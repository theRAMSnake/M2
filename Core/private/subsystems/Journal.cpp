#include "Journal.hpp"
#include "../ObjectManager.hpp"

namespace materia
{

JournalSS::JournalSS(ObjectManager& objMan)
: mOm(objMan)
{

}

void JournalSS::onNewDay()
{

}

void JournalSS::onNewWeek()
{
   
}

std::vector<TypeDef> JournalSS::getTypes()
{
    std::vector<TypeDef> result;

    result.push_back({"journal_header", "journal_index", {
        {"parentFolderId", Type::String},
        {"title", Type::String},
        {"modified", Type::Timestamp},
        {"isPage", Type::Bool}
        }});
    result.back().handlers.onBeforeDelete = std::bind(&JournalSS::handleJournalHeaderDeleted, this, std::placeholders::_1);

    result.push_back({"journal_content", "journal_content_new", {
        {"headerId", Type::String},
        {"content", Type::String}
        }});
    result.back().handlers.onBeforeDelete = std::bind(&JournalSS::handleJournalContentDeleted, this, std::placeholders::_1);
    result.back().handlers.onChanged = std::bind(&JournalSS::handleJournalContentItemChange, this, std::placeholders::_1);
    result.back().handlers.onCreated = std::bind(&JournalSS::handleJournalContentCreated, this, std::placeholders::_1);

    return result;
}

void JournalSS::handleJournalContentItemChange(Object& obj)
{
    auto headerId = obj["headerId"].toId();
    auto header = mOm.get(headerId);

    (*header)["modified"] = Time{std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())};

    mOm.modify(*header);
}

void JournalSS::handleJournalContentDeleted(Object& obj)
{
    auto headerId = obj["headerId"].toId();
    auto header = mOm.get(headerId);

    (*header)["modified"] = Time{std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())};
    (*header)["isPage"] = false;

    mOm.modify(*header);
}

void JournalSS::handleJournalHeaderDeleted(Object& obj)
{
    auto headerId = obj.getId();

    for(auto o : mOm.getAll("journal_content"))
    {
        if((*o)["headerId"].toId() == headerId)
        {
            mOm.destroy(o->getId());
            break;
        }
    }

    for(auto o : mOm.getAll("journal_header"))
    {
        if((*o)["parentFolderId"].toId() == headerId)
        {
            mOm.destroy(o->getId());
        }
    }
}

void JournalSS::handleJournalContentCreated(Object& obj)
{
    auto headerId = obj["headerId"].toId();
    auto header = mOm.get(headerId);

    (*header)["modified"] = Time{std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())};
    (*header)["isPage"] = true;

    mOm.modify(*header);
}

std::vector<CommandDef> JournalSS::getCommandDefs()
{
    return {};
}

}