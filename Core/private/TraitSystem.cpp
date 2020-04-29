#include "TraitSystem.hpp"
#include "JsonSerializer.hpp"
#include <fmt/format.h>

BIND_JSON2(materia::TraitDef, name, isCoreTrait)

namespace materia
{

std::vector<TraitDef> gCoreTypes = {
    {"trait", true}
};

TraitSystem::TraitSystem(Database& db)
: mStorage(db.getTable("traits"))
{

}

Id getId(const std::string& name)
{
    return Id("@@@" + name);
}

std::string fromId(const Id id)
{
    return id.getGuid().substr(3);
}

std::optional<TraitDef> TraitSystem::get(const Id id) const
{
    return get(fromId(id));
}

std::optional<TraitDef> TraitSystem::get(const std::string& name) const
{
    auto loaded = mStorage->load(getId(name));
    if(loaded)
    {
        return readJson<TraitDef>(*loaded);
    }

    return std::optional<TraitDef>();
}

std::vector<TraitDef> TraitSystem::get() const
{
    std::vector<TraitDef> result;

    mStorage->foreach([&](std::string id, std::string json) 
    {
        result.push_back(readJson<TraitDef>(json));
    });

    result.insert(result.end(), gCoreTypes.begin(), gCoreTypes.end());
    return result;
}

Id TraitSystem::add(const TraitDef& newType)
{
    if(get(newType.name))
    {
        throw std::runtime_error(fmt::format("The trait {} already exist", newType.name));
    }

    auto id = getId(newType.name);
    mStorage->store(id, writeJson(newType));
    return id;
}

bool TraitSystem::remove(const std::string& name)
{
    if(name == "trait")
    {
        throw std::runtime_error("Core types cannot be removed");
    }

    if(get(getId(name)))
    {
        mStorage->erase(getId(name));
        return true;
    }
    return false;
}

bool TraitSystem::remove(const Id id)
{
    if(get(id))
    {
        mStorage->erase(id);
        return true;
    }
    return false;
}

boost::property_tree::ptree toPropertyTree(const TraitDef t)
{
    auto pt = readJson<boost::property_tree::ptree>(writeJson(t));
    pt.put("id", getId(t.name));

    boost::property_tree::ptree subParams;

    boost::property_tree::ptree p;
    p.put("", "trait");
    subParams.push_back({"", p});

    pt.add_child("traits", subParams);

    return pt;
}

}