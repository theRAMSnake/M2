#include "TypeSystem.hpp"
#include "JsonSerializer.hpp"
#include <fmt/format.hpp>

BIND_JSON2(materia::TypeDef, domain, name)

namespace materia
{

std::vector<TypeDef> gCoreTypes = {
    {"core", "type", true},
    {"core", "entity", true},
};

TypeSystem::TypeSystem(Database& db)
: mStorage(db.getTable("types"))
{

}

Id getId(const std::string& domain, const std::string& name)
{
    return Id(domain + "/" + name);
}

std::optional<TypeDef> TypeSystem::get(const std::string& domain, const std::string& name) const
{
    if(domain == "core")
    {
        for(auto &t : gCoreTypes)
        {
            if(t.name == name)
            {
                return t;
            }
        }
    }
    else
    {
        return mStorage->load(getId(domain, name));
    }

    return std::optional<TypeDef>();
}

std::vector<TypeDef> TypeSystem::get() const
{
    std::vector<TypeDef> result;

    mStorage->foreach([&](std::string id, std::string json) 
    {
        mItems.push_back({id, readJson<TypeDef>(json)});
    });

    result.insert(result.end(), gCoreTypes.begin(), gCoreTypes.end());
    return result;
}

void TypeSystem::add(const TypeDef& newType)
{
    if(newType.domain == "core")
    {
        throw new std::exception("Core types cannot be added");
    }

    if(get(newType.domain, newType.name))
    {
        throw new std::exception(fmt::format("The type {}/{} already exist", newType.domain, newType.name));
    }

    mStorage->store(getId(newtype.domain, newtype.name), writeJson(newItem));
}

void TypeSystem::remove(const std::string& domain, const std::string& name)
{
    if(newType.domain == "core")
    {
        throw new std::exception("Core types cannot be removed");
    }

    mStorage->erase(getId(newtype.domain, newtype.name));
}

}