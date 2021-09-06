#pragma once
#include "Database.hpp"

namespace materia
{

enum class ConnectionType
{
    Hierarchy
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

};

}
