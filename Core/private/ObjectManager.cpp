#include "ObjectManager.hpp"
#include "JsonSerializer.hpp"
#include "ExceptionsUtil.hpp"
#include "TypeName.hpp"
#include "Connections.hpp"
#include "EmptyValueProvider.hpp"
#include "JsonRestorationProvider.hpp"

namespace materia
{

const unsigned int QUERY_LIMIT = 500;

ObjectManager::ObjectManager(Database& db, TypeSystem& types, Connections& connections)
: mConnections(connections)
, mTypes(types)
{
}

void ObjectManager::initialize(Database& db)
{
    for(auto& t : mTypes.get())
    {
        mStorages[t.name] = db.getTable(t.tableName);
        mStorages[t.name]->foreach([&](std::string id, std::string json)
        {
            try
            {
                JsonRestorationProvider p(json);
                Object newObj(t, id);
                p.populate(newObj);

                mPool.insert({Id(id), newObj});
            }
            catch(std::exception& ex)
            {
                throw std::runtime_error("Type handler initialization failed: Failed to restore object: " + json + " reason: " + ex.what());
            }
        });
    }
}

Connections& ObjectManager::getConnections()
{
    return mConnections;
}

Object ObjectManager::create(const std::optional<Id> id, const std::string& type, const IValueProvider& provider)
{
    auto tp = mTypes.get(type);
    if(!tp)
    {
        throw std::runtime_error(fmt::format("Wrong type while creating object {}", type));
    }

    if(id && mPool.contains(*id))
    {
        throw std::runtime_error(fmt::format("Object with id {} already exist", id->getGuid()));
    }

    auto newId = id ? *id : Id::generate();
    Object newObj(*tp, newId);

    provider.populate(newObj);

    newObj["modified"] = Time{std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())};

    mStorages[type]->store(newId, newObj.toJson());
    mPool.insert({newId, newObj});

    return newObj;
}

std::vector<Object> ObjectManager::query(const std::vector<Id>& ids)
{
    std::vector<Object> objects;
    for(auto id : ids)
    {
        auto pos = mPool.find(id);
        if(pos == mPool.end())
        {
            //It is not good to silently return nothing, but keeping it for compatibility
            //throw std::runtime_error(fmt::format("Object with id {} does not exist", id.getGuid()));
        }
        else
        {
            objects.push_back(pos->second);
        }
    }

    if(objects.size() > QUERY_LIMIT)
    {
        throw std::runtime_error("Query result is greater then 500");
    }

    return objects;
}

std::vector<Object> ObjectManager::query(const Filter& filter)
{
    std::vector<Object> objects;

    v2::InterpreterContext ctx(mConnections);
    for(const auto& kv : mPool)
    {
        try
        {
            ctx.setObject(kv.second);
            if(std::get<bool>(filter.evaluate(ctx)))
            {
                objects.push_back(kv.second);
            }
        }
        catch(...)
        {
            //Do nothing if expession cannot be evaluated for this object
        }
    }

    if(objects.size() > QUERY_LIMIT)
    {
        throw std::runtime_error("Query result is greater then 500");
    }
    return objects;
}

void ObjectManager::destroy(const Id id)
{
    mConnections.remove(id);

    auto pos = mPool.find(id);
    if(pos == mPool.end())
    {
        return;
    }

    auto typeName = pos->second.getType().name;

    auto connections = mConnections.get(id);

    //Remove all children and extensions
    for(auto c : connections)
    {
        if(c.a == id && (c.type == ConnectionType::Hierarchy || c.type == ConnectionType::Extension))
        {
            destroy(c.b);
        }
    }

    connections = mConnections.get(id);

    //Clear all connections
    for(auto c : connections)
    {
        mConnections.remove(c.id);
    }

    mPool.erase(id);
    mStorages[typeName]->erase(id);
}

void ObjectManager::modify(const Id id, const IValueProvider& provider)
{
    auto pos = mPool.find(id);
    if(pos == mPool.end())
    {
        throw std::runtime_error(fmt::format("Object with id {} does not exist", id.getGuid()));
    }

    Object newObj(pos->second);
    provider.populate(newObj);

    newObj["modified"] = Time{std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())};

    mStorages[pos->second.getType().name]->store(id, newObj.toJson());
    pos->second = newObj;

    //Backpropagade extensions
    auto connections = mConnections.get(id);

    for(auto c : connections)
    {
        if(c.b == id && c.type == ConnectionType::Extension)
        {
            modify(get(c.a));
            break; //One extension only expected
        }
    }
}

void ObjectManager::modify(const Object& obj)
{
    auto pos = mPool.find(obj.getId());
    if(pos == mPool.end())
    {
        throw std::runtime_error(fmt::format("Object with id {} does not exist", obj.getId().getGuid()));
    }

    Object newObj(obj);
    newObj["modified"] = Time{std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())};

    mStorages[pos->second.getType().name]->store(obj.getId(), newObj.toJson());
    pos->second = newObj;

    //Backpropagade extensions
    auto connections = mConnections.get(obj.getId());

    for(auto c : connections)
    {
        if(c.b == obj.getId() && c.type == ConnectionType::Extension)
        {
            modify(get(c.a));
            break; //One extension only expected
        }
    }
}

std::vector<Object> ObjectManager::describe() const
{
    std::vector<Object> result;

    auto objType = mTypes.get("object");
    for(auto t : mTypes.get())
    {
        Object obj(*objType, Id::Invalid);
        obj["name"] = t.name;

        std::vector<Object> fields;

        for(auto f : t.fields)
        {
            Object field(*objType, Id::Invalid);
            field["name"] = f.name;
            field["type"] = to_string(f.type);
            field["options"] = f.options;
            field["refType"] = f.refType;

            fields.push_back(field);
        }
        obj.setChildren("fields", fields);

        std::vector<Object> pins;
        for(auto f : t.pins)
        {
            Object pin(*objType, Id::Invalid);
            pin["typeNameOther"] = f.typeNameOther;
            pin["type"] = toString(f.type);
            pin["description"] = f.description;

            pins.push_back(pin);
        }

        obj.setChildren("pins", pins);

        result.push_back(obj);
    }

    return result;
}

Object ObjectManager::get(const Id id)
{
    auto pos = mPool.find(id);
    if(pos == mPool.end())
    {
        throw std::runtime_error(fmt::format("Object with id {} does not exist", id.getGuid()));
    }

    return pos->second;
}

Object ObjectManager::getOrCreate(const Id id, const std::string& type)
{
    auto pos = mPool.find(id);
    if(pos == mPool.end())
    {
        EmptyValueProvider provider;
        return create(id, type, provider);
    }

    return pos->second;
}

std::vector<Object> ObjectManager::getAll(const std::string& type)
{
    std::vector<Object> result;
    for(const auto& kv : mPool)
    {
        if(kv.second.getType().name == type)
        {
            result.push_back(kv.second);
        }
    }
    return result;
}

}
