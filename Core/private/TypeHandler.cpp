#include "TypeHandler.hpp"
#include "JsonRestorationProvider.hpp"
#include <future>

namespace materia
{

TypeHandler::TypeHandler(const TypeDef& type, Database& db, std::function<void(Object&)> onChangeHandler)
: mType(type)
, mStorage(db.getTable(type.tableName))
, mOnChangeHandler(onChangeHandler)
{
    mStorage->foreach([&](std::string id, std::string json) 
    {
        JsonRestorationProvider p(json);
        auto newObj = std::make_shared<Object>(mType, id);
        p.populate(*newObj);

        mPool[Id(id)] = newObj;
    });
}

ObjectPtr TypeHandler::create(const std::optional<Id> id, const IValueProvider& provider)
{
    auto newId = id ? *id : Id::generate();
    auto newObj = std::make_shared<Object>(mType, newId);

    provider.populate(*newObj);

    auto json = newObj->toJson();

    std::async(std::launch::async, [newId, json, this]{ mStorage->store(newId, json); });
    mPool[newId] = newObj;

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
    mPool.erase(id);

    std::async(std::launch::async, [id, this]{ mStorage->erase(id); });   
}

bool TypeHandler::contains(const Id id)
{
    return mPool.contains(id);
}

void TypeHandler::modify(const Id id, const IValueProvider& provider)
{
    auto obj = *get(id);
    provider.populate(*obj);

    mOnChangeHandler(*obj);

    auto json = obj->toJson();

    std::async(std::launch::async, [id, json, this]{ mStorage->store(id, json); });

    mPool[id] = obj;
}

void TypeHandler::modify(const Object& obj)
{
    auto json = obj.toJson();
    auto id = static_cast<Id>(obj["id"]);

    std::async(std::launch::async, [id, json, this]{ mStorage->store(id, json); });
    
    mPool[id] = std::make_shared<Object>(obj);
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