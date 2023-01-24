#pragma once
#include <string>
#include <optional>
#include <vector>
#include <functional>
#include "Type.hpp"
#include "ConnectionType.hpp"

namespace materia
{

struct FieldDef
{
    std::string name;
    Type type;
    std::vector<std::string> options;
    std::string refType;
};

struct EventHandlers
{
};

struct ConnectionPin
{
    std::string typeNameOther;
    ConnectionType type;
    std::string description;
};

struct TypeDef
{
    std::string name;
    std::string tableName;
    std::vector<FieldDef> fields;
    EventHandlers handlers;
    std::vector<ConnectionPin> pins;
};

class TypeSystem
{
public:
    std::optional<TypeDef> get(const std::string& name) const;
    std::vector<TypeDef> get() const;

    void add(const TypeDef& newType);

private:
    std::vector<TypeDef> mTypes;
};

}
