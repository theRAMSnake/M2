#include "TypeHandler.hpp"
#include "JsonRestorationProvider.hpp"
//MH
#include "JsonSerializer.hpp"

namespace materia
{

TypeHandler::TypeHandler(const TypeDef& type, Database& db)
: mType(type)
, mStorage(db.getTable(type.tableName))
{
    mStorage->foreach([&](std::string id, std::string json) 
    {
        mIds.insert(id);
    });

    //Migration hack
    std::vector<ObjectPtr> result;
    std::vector<Id> waste;
    auto slStorage = db.getTable("lists");

    mStorage->foreach([&](std::string id, std::string json) 
    {
        if(json.find("traits") != std::string::npos)
        {
            auto ptree = readJson<boost::property_tree::ptree>(json);
            auto item = ptree.get_child("items");

            std::vector<std::string> items;
            for(auto i : item)
            {
                items.push_back(i.second.get<std::string>("text"));
            }

            TypeDef t = {"simple_list", "lists", {{"objects", Type::Array}}};
            auto newObject = std::make_shared<Object>(t, id);
            (*newObject)["objects"] = items;

            waste.push_back(id);
            slStorage->store(id, newObject->toJson());
        }
    });

    for(auto i : waste)
    {
        mStorage->erase(i);
    }
}

ObjectPtr TypeHandler::create(const std::optional<Id> id, const IValueProvider& provider)
{
    auto newId = id ? *id : Id::generate();
    auto newObj = std::make_shared<Object>(mType, newId);

    provider.populate(*newObj);

    mStorage->store(newId, newObj->toJson());
    mIds.insert(newId);

    return newObj;
}

std::vector<ObjectPtr> TypeHandler::query(const std::vector<Id>& ids)
{
    std::vector<ObjectPtr> result;

    for(auto id : ids)
    {
        auto o = get(id);
        if(o)
        {
            result.push_back(*o);
        }
    }

    return result;
}

std::vector<ObjectPtr> TypeHandler::query(const Filter& f)
{
    std::vector<ObjectPtr> result;

    mStorage->foreach([&](std::string id, std::string json) 
    {   
        JsonRestorationProvider p(json);
        auto newObj = std::make_shared<Object>(mType, id);
        p.populate(*newObj);

        if(std::get<bool>(f.evaluate(*newObj)))
        {
            result.push_back(newObj);
        }
    });

    return result;
}

std::optional<ObjectPtr> TypeHandler::get(const Id& id)
{
    auto obj = mStorage->load(id);

    if(obj)
    {
        JsonRestorationProvider p(*obj);
        auto newObj = std::make_shared<Object>(mType, id);
        p.populate(*newObj);

        return newObj;
    }
    else
    {
        return std::optional<ObjectPtr>();
    }
}

void TypeHandler::destroy(const Id id)
{
    mIds.erase(id);
    mStorage->erase(id);
}

bool TypeHandler::contains(const Id id)
{
    return mIds.find(id) != mIds.end();
}

void TypeHandler::modify(const Id id, const IValueProvider& provider)
{
    auto obj = *get(id);
    provider.populate(*obj);

    mStorage->store(id, obj->toJson());
}

}