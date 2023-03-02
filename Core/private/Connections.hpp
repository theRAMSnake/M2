#pragma once
#include "Database.hpp"
#include "Object.hpp"
#include "ConnectionType.hpp"
#include <unordered_map>
#include <unordered_set>

namespace materia
{

struct Connection
{
    Id id;
    Id a;
    Id b;
    ConnectionType type;
};

bool operator < (const Connection& a, const Connection& b);
bool operator == (const Connection& a, const Connection& b);
struct ConnectionHash
{
    std::size_t operator()(const Connection& c) const
    {
        return std::hash<std::string>()(c.a.getGuid()) +
            std::hash<std::string>()(c.b.getGuid()) + static_cast<std::size_t>(c.type);
    }
};
Connection jsonToConnection(const Id& id, const std::string& json);
Object connectionToObject(const Connection& src);
std::string toString(const ConnectionType& ct);

class Connections
{
public:
    friend class ObjectManager;
    struct Any{};

    Connections(Database& db);

    //It is assumed that objects are being cleaned up as neccessary
    void remove(const Id& id);

    //It is assumed that both a and b exists
    Id create(const Id& a, const Id& b, const ConnectionType type);

    bool contains(const Id& a, const Id& b, const ConnectionType type) const;
    bool contains(const Any& a, const Id& b, const ConnectionType type) const;

    void fetch(const std::vector<Id>& ids, std::vector<Connection>& out) const;
    //Unoptimal - avoid usage
    std::vector<Connection> get(const Id& a) const;

private:
    void validate(const Id& a, const Id& b, const ConnectionType type) const;
    std::optional<Id> getPredecessorOf(const Id& id, const ConnectionType type) const;

    std::unique_ptr<DatabaseTable> mStorage;
    std::unordered_set<Connection, ConnectionHash> mConnections;
};

}
