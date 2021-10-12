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
    
    void remove(const Id& id);
    Id create(const Id& a, const Id& b, const ConnectionType type);
    
    std::vector<Connection> get(const Id& a) const;

private:
    std::unique_ptr<DatabaseTable> mStorage;
    std::vector<Connection> mConnections;
};

}
