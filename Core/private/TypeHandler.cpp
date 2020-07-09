#include "TypeHandler.hpp"
#include "JsonRestorationProvider.hpp"
#include <future>

namespace materia
{

TypeHandler::TypeHandler(
    const TypeDef& type, 
    Database& db, 
    std::function<void(Object&)> onChangeHandler, 
    std::function<void(Object&)> onBeforeDeleteHandler, 
    std::function<void(Object&)> onCreatedHandler
    )
: mType(type)
, mStorage(db.getTable(type.tableName))
, mOnChangeHandler(onChangeHandler)
, mOnBeforeDeleteHandler(onBeforeDeleteHandler)
, mOnCreatedHandler(onCreatedHandler)
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

    postpone(std::async(std::launch::async, [newId, json, this]{ mStorage->store(newId, json); }));
    mPool[newId] = newObj;

    mOnCreatedHandler(*newObj);

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
    mOnBeforeDeleteHandler(*obj);

    mPool.erase(id);
    postpone(std::async(std::launch::async, [id, this]{ mStorage->erase(id); }));      
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

    postpone(std::async(std::launch::async, [id, json, this]{ mStorage->store(id, json); }));

    mPool[id] = obj;
}

void TypeHandler::modify(const Object& obj)
{
    auto json = obj.toJson();
    auto id = static_cast<Id>(obj["id"]);

    postpone(std::async(std::launch::async, [id, json, this]{ mStorage->store(id, json); }));
    
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

void TypeHandler::postpone(std::future<void>&& future)
{
    mOpPool.push_back(std::move(future));
    if(mOpPool.size() > 25)
    {
        mOpPool.erase(mOpPool.begin());
    }
}

}