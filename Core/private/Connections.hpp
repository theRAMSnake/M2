#pragma once
#include "Database.hpp"
#include "Object.hpp"

namespace materia
{

enum class ConnectionType
{
    //A is parent, B is child. Recursive hierarchy is not allowed. B is owned by A.
    Hierarchy,

    //B extends A. There can be only one extension of A. Non recursive. B is owned by A.
    Extension,

    //B refered by A. No restrictions.
    Reference,

    //B requires A. No loops allowed. A and B are both independent of each other.
    Requirement
};

struct Connection
{
    Id id;
    Id a;
    Id b;
    ConnectionType type;
};

bool operator < (const Connection& a, const Connection& b);

Connection jsonToConnection(const Id& id, const std::string& json);
Object connectionToObject(const Connection& src);

class Connections
{
public:
    Connections(Database& db);
    
    //It is assumed that objects are being cleaned up as neccessary
    void remove(const Id& id);

    //It is assumed that both a and b exists
    Id create(const Id& a, const Id& b, const ConnectionType type);
    
    std::vector<Connection> get(const Id& a) const;

private:
    void validate(const Id& a, const Id& b, const ConnectionType type) const;
    std::optional<Id> getPredecessorOf(const Id& id, const ConnectionType type) const;

    std::unique_ptr<DatabaseTable> mStorage;
    std::vector<Connection> mConnections;
};

}
