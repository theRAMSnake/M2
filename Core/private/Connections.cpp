#include "Connections.hpp"
#include <Common/Id.hpp>
#include <fmt/format.h>
#include "JsonRestorationProvider.hpp"

namespace materia
{

Connections::Connections(Database& db)
: mStorage(db.getTable("connections"))
{
    mStorage->foreach([&](std::string id, std::string json) 
    {
        JsonRestorationProvider p(json);
        Object newObj({"connection"}, id);
        p.populate(newObj);

        mConnections.push_back({Id(id), newObj["A"].toId(), newObj["B"].toId(), static_cast<ConnectionType>(newObj["type"].get<Type::Int>())});
    });
}

void Connections::remove(const Id& id)
{
    mConnections.erase(find_by_id(mConnections, id));
    mStorage->erase(id);
}

void Connections::validate(const Id& a, const Id& b, const ConnectionType type) const
{
    if(a == b)
    {
        throw std::runtime_error("Cannot create connection with a == b");
    }

    auto pos = std::find_if(mConnections.begin(), mConnections.end(), [&](auto x){
            return x.a == a && x.b == b && x.type == type;
        });

    if(pos != mConnections.end())
    {
        throw std::runtime_error(fmt::format("Connection already exists: {} to {} of type {}", a.getGuid(), b.getGuid(), static_cast<int>(type)));
    }

    if(type == ConnectionType::Hierarchy || type == ConnectionType::Extension)
    {
        //Make sure b is not an ancestor of a
        std::optional<Id> curPre = a;
        while(curPre)
        {
            if(*curPre == b)
            {
                throw std::runtime_error(fmt::format("Cannot create connection: {} is already an ancestor of {}", b.getGuid(), a.getGuid()));
            }
            curPre = getPredecessorOf(*curPre, type); 
        }
    }

    if(type == ConnectionType::Extension)
    {
        //Make sure a does not already have an extension
        auto ext = std::find_if(mConnections.begin(), mConnections.end(), [&](auto x){
                return x.a == a && x.type == ConnectionType::Extension;
            });
        if(ext != mConnections.end())
        {
            throw std::runtime_error(fmt::format("Cannot create connection: {} already extended", a.getGuid()));
        }
    }
}

Id Connections::create(const Id& a, const Id& b, const ConnectionType type)
{
    validate(a, b, type);

    auto id = Id::generate();
    mConnections.push_back({id, a, b, type});

    Object serializer({"connection"}, id);
    serializer["A"] = a.getGuid();
    serializer["B"] = b.getGuid();
    serializer["type"] = static_cast<int>(type);

    mStorage->store(id, serializer.toJson());
    return id;
}

std::vector<Connection> Connections::get(const Id& a) const
{
    std::vector<Connection> subset;

    std::copy_if(mConnections.begin(), mConnections.end(), std::back_inserter(subset), [&](auto x)
    {
        return x.a == a || x.b == a;
    });

    return subset;
}

std::optional<Id> Connections::getPredecessorOf(const Id& id, const ConnectionType type) const
{
    for(const auto& i : mConnections)
    {
        if(i.b == id && i.type == type)
        {
            return i.a;
        }
    }

    return {};
}
}
