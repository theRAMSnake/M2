#include "Connections.hpp"
#include <Common/Id.hpp>
#include <fmt/format.h>
#include "JsonRestorationProvider.hpp"
#include <set>

namespace materia
{

ConnectionType toConnectionType(const std::string& str)
{
    if(str == "Hierarchy")
    {
        return ConnectionType::Hierarchy;
    }
    else if(str == "Extension")
    {
        return ConnectionType::Extension;
    }
    else if(str == "Reference")
    {
        return ConnectionType::Reference;
    }
    else if(str == "Requirement")
    {
        return ConnectionType::Requirement;
    }
    else
    {
        throw std::runtime_error("Unknown connection type: " + str);
    }
}

Connection jsonToConnection(const Id& id, const std::string& json)
{
    JsonRestorationProvider p(json);
    Object newObj({"connection"}, id);
    p.populate(newObj);

    const Object expected = newObj;

    return {Id(id), expected["A"].toId(), expected["B"].toId(), toConnectionType(expected["type"].get<Type::String>())};
}

Connections::Connections(Database& db)
: mStorage(db.getTable("connections"))
{
    mStorage->foreach([&](std::string id, std::string json)
    {
        auto newConn = jsonToConnection(id, json);
        mConnections.insert(newConn);
    });
}

void Connections::remove(const Id& id)
{
    for(auto iter = mConnections.begin(); iter != mConnections.end(); ++iter)
    {
        if(iter->id == id)
        {
            mConnections.erase(iter);
            mStorage->erase(id);
            break;
        }
    }
}

void Connections::validate(const Id& a, const Id& b, const ConnectionType type) const
{
    if(a == b)
    {
        throw std::runtime_error("Cannot create connection with a == b");
    }

    auto subset = get(a);
    auto pos = std::find_if(subset.begin(), subset.end(), [&](auto x){
            return x.b == b && x.type == type;
        });

    if(pos != subset.end())
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
        auto ext = std::find_if(subset.begin(), subset.end(), [&](auto x){
                return x.a == a && x.type == ConnectionType::Extension;
            });
        if(ext != subset.end())
        {
            throw std::runtime_error(fmt::format("Cannot create connection: {} already extended", a.getGuid()));
        }
    }

    if(type == ConnectionType::Requirement)
    {
        //Make sure no loops exist => Go forward from b and make sure a is not reached.
        std::set<Id> toVisit;
        toVisit.insert(b);

        while(!toVisit.empty())
        {
            auto cur = *toVisit.begin();
            toVisit.erase(toVisit.begin());

            for(auto c : get(cur))
            {
                if(c.type == ConnectionType::Requirement && c.a == cur)
                {
                    if(c.b == a)
                    {
                        throw std::runtime_error(fmt::format("Cannot create connection: {} already a requirement for {}", b.getGuid(), a.getGuid()));
                    }
                    toVisit.insert(c.b);
                }
            }
        }
    }
}

std::string toString(const ConnectionType& ct)
{
    if(ct == ConnectionType::Hierarchy)
    {
        return "Hierarchy";
    }
    else if(ct == ConnectionType::Extension)
    {
        return "Extension";
    }
    else if(ct == ConnectionType::Reference)
    {
        return "Reference";
    }
    else if(ct == ConnectionType::Requirement)
    {
        return "Requirement";
    }
    else
    {
        throw std::runtime_error("Unhandled connection type: " + std::to_string(static_cast<int>(ct)));
    }
}

bool operator < (const Connection& a, const Connection& b)
{
    return std::make_tuple(a.type, a.a, a.b) < std::make_tuple(b.type, b.a, b.b);
}
bool operator == (const Connection& a, const Connection& b)
{
    return std::make_tuple(a.type, a.a, a.b) == std::make_tuple(b.type, b.a, b.b);
}


Object connectionToObject(const Connection& src)
{
    Object result({"connection"}, src.id);
    result["A"] = src.a.getGuid();
    result["B"] = src.b.getGuid();
    result["type"] = toString(src.type);

    return result;
}

Id Connections::create(const Id& a, const Id& b, const ConnectionType type)
{
    validate(a, b, type);

    auto id = Id::generate();
    auto newConn = Connection{id, a, b, type};
    mConnections.insert(newConn);

    mStorage->store(id, connectionToObject(newConn).toJson());
    return id;
}

std::vector<Connection> Connections::get(const Id& a) const
{
    std::vector<Connection> result;

    for(const auto& i : mConnections)
    {
        if(i.b == a || i.a == a)
        {
            result.push_back(i);
        }
    }

    return result;
}

bool Connections::contains(const Id& a, const Id& b, const ConnectionType type) const
{
    return mConnections.contains(Connection{materia::Id(), a, b, type});
}

bool Connections::contains(const Any& a, const Id& b, const ConnectionType type) const
{
    for(const auto& i : mConnections)
    {
        if(i.b == b && i.type == type)
        {
            return true;
        }
    }

    return false;
}

void Connections::fetch(const std::vector<Id>& ids, std::vector<Connection>& out) const
{
    for(const auto& id : ids)
    {
        for(const auto& i : mConnections)
        {
            if(i.b == id || i.a == id)
            {
                out.push_back(i);
            }
        }
    }
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
