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
            Object newObj(mType, id);
            p.populate(newObj);

            mPool.insert({Id(id), newObj});
        }
        catch(std::exception& ex)
        {
            throw std::runtime_error("Type handler initialization failed: Failed to restore object: " + json + " reason: " + ex.what());
        }
    });
}

Object TypeHandler::create(const std::optional<Id> id, const IValueProvider& provider)
{
    auto newId = id ? *id : Id::generate();
    Object newObj(mType, newId);

    provider.populate(newObj);
    mType.handlers.onValidation(newObj);

    newObj["modified"] = Time{std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())};

    mStorage->store(newId, newObj.toJson());
    mPool.insert({newId, newObj});

    mType.handlers.onCreated(newObj);

    return newObj;
}

std::vector<Object> TypeHandler::query(const std::vector<Id>& ids)
{
    std::vector<Object> result;

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

std::vector<Object> TypeHandler::query(const Filter& f, const Connections& cons)
{
    std::vector<Object> result;

    for(auto kv : mPool) 
    {   
        try
        {
            if(std::get<bool>(f.evaluate(kv.second, cons)))
            {
                result.push_back(kv.second);
            }
        }
        catch(...)
        {
            //Do nothing if expession cannot be evaluated for this object
        }
    }

    return result;
}

std::optional<Object> TypeHandler::get(const Id& id)
{
    auto pos = mPool.find(id);

    if(pos != mPool.end())
    {
        return pos->second;
    }
    else
    {
        return std::optional<Object>();
    }
}

void TypeHandler::destroy(const Id id)
{
    auto obj = *get(id);
    mType.handlers.onBeforeDelete(obj);

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
    Object newObj(obj);
    provider.populate(newObj);

    mType.handlers.onChanging(obj, newObj);

    newObj["modified"] = Time{std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())};

    mStorage->store(id, newObj.toJson());
    mPool.find(id)->second = newObj;

    mType.handlers.onChanged(newObj);
}

void TypeHandler::modify(const Object& obj)
{
    auto oldObj = *get(obj.getId());
    Object newObj(obj);

    mType.handlers.onChanging(oldObj, newObj);

    newObj["modified"] = Time{std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())};

    mStorage->store(obj.getId(), obj.toJson());    
    mPool.find(obj.getId())->second = newObj;

    mType.handlers.onChanged(newObj);
}

std::vector<Object> TypeHandler::getAll()
{
    std::vector<Object> result;

    for(auto kv : mPool) 
    {   
        result.push_back(kv.second);
    }

    return result;
}

}
