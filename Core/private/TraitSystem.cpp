#include "TraitSystem.hpp"
#include "JsonSerializer.hpp"
#include <fmt/format.h>

BIND_JSON3(materia::TraitDef, name, isCoreTrait, requires)
BIND_JSON2(materia::Require, field, type)

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
    boost::property_tree::ptree result;

    result.put("id", getId(t.name));
    result.put("name", t.name);
    result.put("isCoreTrait", t.isCoreTrait);

    {
        boost::property_tree::ptree subParams;

        boost::property_tree::ptree p;
        p.put("", "trait");
        subParams.push_back({"", p});

        result.add_child("traits", subParams);
    }
    {
        boost::property_tree::ptree subParams;

        for(auto r : t.requires)
        {
            boost::property_tree::ptree p;
            p.put("field", r.field);
            p.put("type", r.type);
            subParams.push_back({"", p});
        }

        result.add_child("requires", subParams);
    }

    return result;
}

TraitDef fromPropertyTree(const boost::property_tree::ptree& ptree)
{
    try
    {
        TraitDef result = { ptree.get<std::string>("name"), ptree.get<bool>("isCoreTrait") };

        auto r = ptree.get_child_optional("requires");

        if(r)
        {
            for(auto& v : *r)
            {
                result.requires.push_back({v.second.get<std::string>("field"), v.second.get<std::string>("type")});
            }
        }
        
        return result;
    }
    catch(...)
    {
        throw std::runtime_error("Cannot convert prop tree to TraitDef");
    }
}

void TraitSystem::edit(const TraitDef& type)
{
    mStorage->store(getId(type.name), writeJson(type));
}

}