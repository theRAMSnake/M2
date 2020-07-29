#include "TypeHandler.hpp"
#include "JsonRestorationProvider.hpp"
#include <future>
#include "Logger.hpp"

namespace materia
{

TypeHandler::TypeHandler(
    const TypeDef& type, 
    Database& db
    )
: mType(type)
, mStorage(db.getTable(type.tableName))
{
    mStorage->foreach([&](std::string id, std::string json) 
    {
        try
        {
            JsonRestorationProvider p(json);
            auto newObj = std::make_shared<Object>(mType, id);
            p.populate(*newObj);

            mPool[Id(id)] = newObj;
        }
        catch(...)
        {
            LOG("Failed to restore object");
        }
    });
}

ObjectPtr TypeHandler::create(const std::optional<Id> id, const IValueProvider& provider)
{
    auto newId = id ? *id : Id::generate();
    auto newObj = std::make_shared<Object>(mType, newId);

    provider.populate(*newObj);

    mStorage->store(newId, newObj->toJson());
    mPool[newId] = newObj;

    mType.handlers.onCreated(*newObj);

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

    for(auto kv : mPool) 
    {   
        if(std::get<bool>(f.evaluate(*kv.second)))
        {
            result.push_back(kv.second);
        }
    }

    return result;
}

std::optional<ObjectPtr> TypeHandler::get(const Id& id)
{
    auto pos = mPool.find(id);

    if(pos != mPool.end())
    {
        return pos->second;
    }
    else
    {
        return std::optional<ObjectPtr>();
    }
}

void TypeHandler::destroy(const Id id)
{
    auto obj = *get(id);
    mType.handlers.onBeforeDelete(*obj);

    mPool.erase(id);
    mStorage->erase(id);      
}

bool TypeHandler::contains(const Id id)
{
    return mPool.contains(id);
}

void TypeHandler::modify(const Id id, const IValueProvider& provider)
{
    auto obj = *get(id);
    provider.populate(*obj);

    mType.handlers.onChanged(*obj);

    mStorage->store(id, obj->toJson());
    mPool[id] = obj;
}

void TypeHandler::modify(const Object& obj)
{
    mStorage->store(obj.getId(), obj.toJson());    
    mPool[obj.getId()] = std::make_shared<Object>(obj);
}

std::vector<ObjectPtr> TypeHandler::getAll()
{
    std::vector<ObjectPtr> result;

    for(auto kv : mPool) 
    {   
        result.push_back(kv.second);
    }

    return result;
}

}