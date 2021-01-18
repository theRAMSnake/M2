#include "TypeHandler.hpp"
#include "JsonRestorationProvider.hpp"
#include <future>
#include "Logger.hpp"

namespace materia
{

bool choiceAndOptionBinderPatch(Object& obj)
{
    return obj.choiceAndOptionBinderPatch();
}

TypeHandler::TypeHandler(
    const TypeDef& type, 
    Database& db
    )
: mType(type)
, mStorage(db.getTable(type.tableName))
{
    bool needOverwrite = false;
    mStorage->foreach([&](std::string id, std::string json) 
    {
        try
        {
            JsonRestorationProvider p(json);
            Object newObj(mType, id);
            p.populate(newObj);

            auto patchRes = choiceAndOptionBinderPatch(newObj);
            needOverwrite = needOverwrite || patchRes;

            mPool.insert({Id(id), newObj});
        }
        catch(...)
        {
            throw std::runtime_error("Type handler initialization failed: Failed to restore object: " + json);
        }
    });

    if(needOverwrite)
    {
        for(auto kv : mPool) 
        {   
            mStorage->store(kv.first, kv.second.toJson());
        }
    }
}

Object TypeHandler::create(const std::optional<Id> id, const IValueProvider& provider)
{
    auto newId = id ? *id : Id::generate();
    Object newObj(mType, newId);

    provider.populate(newObj);
    choiceAndOptionBinderPatch(newObj);
    mType.handlers.onValidation(newObj);

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

std::vector<Object> TypeHandler::query(const Filter& f)
{
    std::vector<Object> result;

    for(auto kv : mPool) 
    {   
        try
        {
            if(std::get<bool>(f.evaluate(kv.second)))
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

    choiceAndOptionBinderPatch(newObj);

    mType.handlers.onChanging(obj, newObj);

    mStorage->store(id, newObj.toJson());
    mPool.find(id)->second = newObj;

    mType.handlers.onChanged(newObj);
}

void TypeHandler::modify(const Object& obj)
{
    auto oldObj = *get(obj.getId());
    Object newObj(obj);

    choiceAndOptionBinderPatch(newObj);
    mType.handlers.onChanging(oldObj, newObj);

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