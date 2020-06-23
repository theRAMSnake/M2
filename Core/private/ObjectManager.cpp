#include "ObjectManager.hpp"
#include "JsonSerializer.hpp"
#include "ExceptionsUtil.hpp"
#include "TypeName.hpp"
#include "EmptyValueProvider.hpp"

namespace materia
{

const unsigned int QUERY_LIMIT = 50;

void ObjectManager::handleChItemChange(Object& obj)
{
    while(static_cast<int>(obj["points"]) > static_cast<int>(obj["pointsNeeded"]))
    {
        obj["points"] = static_cast<int>(obj["points"]) - static_cast<int>(obj["pointsNeeded"]);
        obj["level"] = static_cast<int>(obj["level"]) + 1;
        obj["pointsNeeded"] = static_cast<int>(obj["pointsNeeded"]) + static_cast<int>(obj["advance"]);

        mReward.addPoints(static_cast<int>(obj["rewardPerLevel"]));
    }
}

void doNothing(Object& obj)
{

}

std::function<void(Object&)> ObjectManager::createOnChangeHandler(const std::string& typeName)
{
    if(typeName == "challenge_item")
    {
        return std::bind(&ObjectManager::handleChItemChange, this, std::placeholders::_1);
    }
    else
    {
        return std::function<void(Object&)>(doNothing);
    }
}

ObjectManager::ObjectManager(Database& db, TypeSystem& types, IReward& reward)
: mDb(db)
, mTypes(types)
, mReward(reward)
{
    for(auto& t : mTypes.get())
    {
        mHandlers[t.name] = std::make_shared<TypeHandler>(t, mDb, createOnChangeHandler(t.name));
    }
}

IReward& ObjectManager::LEGACY_getReward()
{
    return mReward;
}

ObjectPtr ObjectManager::create(const std::optional<Id> id, const std::string& type, const IValueProvider& provider)
{
    auto pos = mHandlers.find(type); 
    if(pos == mHandlers.end())
    {
        throw std::runtime_error(fmt::format("Wrong type while creating object {}", type));
    }

    if(id)
    {
        for(auto h : mHandlers)
        {
            if(h.second->contains(*id))
            {
                throw std::runtime_error(fmt::format("Object with id {} already exist", id->getGuid()));
            }
        }
    }

    return pos->second->create(id, provider);
}

std::vector<ObjectPtr> ObjectManager::query(const std::vector<Id>& ids)
{
    std::vector<ObjectPtr> objects;

    for(auto& h : mHandlers)
    {
        auto newObjects = h.second->query(ids);
        objects.insert(objects.end(), newObjects.begin(), newObjects.end());

        if(objects.size() > QUERY_LIMIT)
        {
            break;
        }
    }

    return objects;
}

std::vector<ObjectPtr> ObjectManager::query(const Filter& filter)
{
    std::vector<ObjectPtr> objects;

    for(auto& h : mHandlers)
    {
        auto newObjects = h.second->query(filter);
        objects.insert(objects.end(), newObjects.begin(), newObjects.end());

        if(objects.size() > QUERY_LIMIT)
        {
            break;
        }
    }

    return objects;
}

void ObjectManager::destroy(const Id id)
{
    for(auto& h : mHandlers)
    {
        if(h.second->contains(id))
        {
            h.second->destroy(id);
            break;
        }
    }
}

void ObjectManager::modify(const Id id, const IValueProvider& provider)
{
    for(auto& h : mHandlers)
    {
        if(h.second->contains(id))
        {
            h.second->modify(id, provider);
            break;
        }
    }
}

void ObjectManager::modify(const Object& obj)
{
    for(auto& h : mHandlers)
    {
        if(h.second->contains(static_cast<Id>(obj["id"])))
        {
            h.second->modify(obj);
            break;
        }
    }
}

std::string to_string(const Type t)
{
    switch(t)
    {   
        case Type::Int: return "int";
        case Type::Money: return "money";
        case Type::Double: return "double";
        case Type::Bool: return "bool";
        case Type::String: return "string";
        case Type::Reference: return "reference";
        case Type::Array: return "array";
        case Type::Timestamp: return "timestamp";
        case Type::Option: return "option";
    }

    throw std::runtime_error("unknown type");
}

std::vector<ObjectPtr> ObjectManager::describe() const
{
    std::vector<ObjectPtr> result;

    auto objType = mTypes.get("object");
    for(auto t : mTypes.get())
    {
        auto obj = std::make_shared<Object>(*objType, Id::Invalid);
        (*obj)["name"] = t.name;

        std::vector<ObjectPtr> fields;

        for(auto f : t.fields)
        {
            auto field = std::make_shared<Object>(*objType, Id::Invalid);
            (*field)["name"] = f.name;
            (*field)["type"] = to_string(f.type);
            (*field)["options"] = f.options;

            fields.push_back(field);
        }

        (*obj)["fields"] = fields;

        result.push_back(obj);
    }

    return result;
}

ObjectPtr ObjectManager::get(const Id id)
{
    for(auto& h : mHandlers)
    {
        if(h.second->contains(id))
        {
            return *h.second->get(id);
        }
    }

    throw std::runtime_error(fmt::format("Object with id {} not found", id.getGuid()));
}

ObjectPtr ObjectManager::getOrCreate(const Id id, const std::string& type)
{
    for(auto& h : mHandlers)
    {
        if(h.second->contains(id))
        {
            return *h.second->get(id);
        }
    }

    EmptyValueProvider provider;
    create(id, type, provider);

    return get(id);
}

std::vector<ObjectPtr> ObjectManager::getAll(const std::string& type)
{
    if(mHandlers.contains(type))
    {
        return mHandlers[type]->getAll();
    }

    return {};
}

}