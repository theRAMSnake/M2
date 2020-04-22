#include "TypeSystem.hpp"
#include "JsonSerializer.hpp"
#include <fmt/format.h>

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

std::tuple<std::string, std::string> fromId(const Id id)
{
    auto guid = id.getGuid();
    auto pos = guid.find('/');
    return {guid.substr(0, pos), guid.substr(pos + 1)};
}

std::optional<TypeDef> TypeSystem::get(const Id id) const
{
    auto [domain, name] = fromId(id);
    return get(domain, name);
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
        auto loaded = mStorage->load(getId(domain, name));
        if(loaded)
        {
            return readJson<TypeDef>(*loaded);
        }
    }

    return std::optional<TypeDef>();
}

std::vector<TypeDef> TypeSystem::get() const
{
    std::vector<TypeDef> result;

    mStorage->foreach([&](std::string id, std::string json) 
    {
        result.push_back(readJson<TypeDef>(json));
    });

    result.insert(result.end(), gCoreTypes.begin(), gCoreTypes.end());
    return result;
}

Id TypeSystem::add(const TypeDef& newType)
{
    if(newType.domain == "core")
    {
        throw std::runtime_error("Core types cannot be added");
    }

    if(get(newType.domain, newType.name))
    {
        throw std::runtime_error(fmt::format("The type {}/{} already exist", newType.domain, newType.name));
    }

    onAdded(newType);

    auto id = getId(newType.domain, newType.name);
    mStorage->store(id, writeJson(newType));
    return id;
}

void TypeSystem::remove(const std::string& domain, const std::string& name)
{
    if(domain == "core")
    {
        throw std::runtime_error("Core types cannot be removed");
    }

    mStorage->erase(getId(domain, name));
}

void TypeSystem::remove(const Id id)
{
    mStorage->erase(id);
}

boost::property_tree::ptree toPropertyTree(const TypeDef t)
{
    return readJson<boost::property_tree::ptree>(writeJson(t));
}

}