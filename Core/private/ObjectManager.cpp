#include "ObjectManager.hpp"
#include "JsonSerializer.hpp"
#include "ExceptionsUtil.hpp"
#include "TypeName.hpp"

namespace materia
{

const unsigned int QUERY_LIMIT = 50;

ObjectManager::ObjectManager(Database& db, TypeSystem& types)
: mDb(db)
, mTypes(types)
{
    for(auto& t : mTypes.get())
    {
        mHandlers[t.name] = std::make_shared<TypeHandler>(t, mDb);
    }
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

}