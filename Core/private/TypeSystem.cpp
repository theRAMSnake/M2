#include "TypeSystem.hpp"
#include <fmt/format.h>

namespace materia
{

TypeSystem::TypeSystem()
{
    mTypes.push_back({"object", "objects"});
    mTypes.push_back({"variable", "variables", {{"value", Type::String}}});
    mTypes.push_back({"simple_list", "lists", {{"objects", Type::Array}}});
    mTypes.push_back({"calendar_item", "calendar", {
        {"text", Type::String},
        {"timestamp", Type::Timestamp},
        {"reccurencyType", Type::Option, {"None", "Weekly", "Monthly", "Quarterly", "Yearly"}},
        {"entityType", Type::Option, {"Event", "Task"}}
        }});
    mTypes.push_back({"challenge_item", "challenge_new", {
        {"brief", Type::String},
        {"points", Type::Int},
        {"pointsNeeded", Type::Int},
        {"level", Type::Int},
        {"advance", Type::Int},
        {"rewardPerLevel", Type::Int},
        {"resetWeekly", Type::Bool},
        }});
    mTypes.push_back({"finance_category", "finance_categories", {{"name", Type::String}}});
    mTypes.push_back({"finance_event", "finance_events", {
        {"categoryId", Type::Reference, {}, "finance_category"},
        {"type", Type::Option, {"Spending", "Earning"}},
        {"details", Type::String},
        {"amountEuroCents", Type::Money},
        {"timestamp", Type::Timestamp}
        }});
    mTypes.push_back({"journal_header", "journal_index", {
        {"parentFolderId", Type::String},
        {"title", Type::String},
        {"modified", Type::Timestamp},
        {"isPage", Type::Bool}
        }});
    mTypes.push_back({"journal_content", "journal_content_new", {
        {"headerId", Type::String},
        {"content", Type::String}
        }});
}

std::optional<TypeDef> TypeSystem::get(const std::string& name) const
{
    auto pos = std::find_if(mTypes.begin(), mTypes.end(), [name](auto x){return x.name == name;});
    
    if(pos != mTypes.end())
    {
        return *pos;
    }

    return std::optional<TypeDef>();
}

std::vector<TypeDef> TypeSystem::get() const
{
    return mTypes;
}

void TypeSystem::add(const TypeDef& newType)
{
    if(get(newType.name))
    {
        throw std::runtime_error("Trying to add type which already exist");
    }

    mTypes.push_back(newType);
}

}