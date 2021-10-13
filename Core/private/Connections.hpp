#pragma once
#include "Database.hpp"

namespace materia
{

enum class ConnectionType
{
    //A is parent, B is child. Recursive hierarchy is not allowed. B is owned by A.
    Hierarchy,

    //B extends A. There can be only one extension of A. Non recursive. B is owned by A.
    Extension,

    //B refers to A. No restrictions.
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

class Connections
{
public:
    Connections(Database& db);
    
    //It is assumed that object are being cleaned up as neccessary
    void remove(const Id& id);

    //It is assumed that both a and b exists
    Id create(const Id& a, const Id& b, const ConnectionType type);
    
    std::vector<Connection> get(const Id& a) const;

private:
    void validate(const Id& a, const Id& b, const ConnectionType type) const;
    std::optional<Id> getParentOf(const Id& id) const;

    std::unique_ptr<DatabaseTable> mStorage;
    std::vector<Connection> mConnections;
};

}
