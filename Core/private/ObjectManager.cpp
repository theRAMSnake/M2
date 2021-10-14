#include "ObjectManager.hpp"
#include "JsonSerializer.hpp"
#include "ExceptionsUtil.hpp"
#include "TypeName.hpp"
#include "Connections.hpp"
#include "EmptyValueProvider.hpp"

namespace materia
{

const unsigned int QUERY_LIMIT = 50;

ObjectManager::ObjectManager(Database& db, TypeSystem& types, Connections& connections)
: mDb(db)
, mConnections(connections)
, mTypes(types)
{
    
}

void ObjectManager::initialize()
{
    for(auto& t : mTypes.get())
    {
        mHandlers[t.name] = std::make_shared<TypeHandler>(t, mDb);
    }
}

Object ObjectManager::create(const std::optional<Id> id, const std::string& type, const IValueProvider& provider)
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

std::vector<Object> ObjectManager::query(const std::vector<Id>& ids)
{
    std::vector<Object> objects;

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

std::vector<Object> ObjectManager::query(const Filter& filter)
{
    std::vector<Object> objects;

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
        if(h.second->contains(obj.getId()))
        {
            h.second->modify(obj);
            break;
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

        result.push_back(obj);
    }

    return result;
}

Object ObjectManager::get(const Id id)
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

Object ObjectManager::getOrCreate(const Id id, const std::string& type)
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

std::vector<Object> ObjectManager::getAll(const std::string& type)
{
    if(mHandlers.contains(type))
    {
        return mHandlers[type]->getAll();
    }

    return {};
}

}
