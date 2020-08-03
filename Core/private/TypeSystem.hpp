#pragma once
#include <string>
#include <optional>
#include <vector>
#include <functional>
#include "Type.hpp"

namespace materia
{

struct FieldDef
{
    std::string name;
    Type type;
    std::vector<std::string> options;
    std::string refType;
};

class Object;
void doNothing(Object& obj);

struct EventHandlers
{
    std::function<void(Object&)> onChanged = doNothing;
    std::function<void(Object&)> onBeforeDelete = doNothing; 
    std::function<void(Object&)> onCreated = doNothing;
};

struct TypeDef
{
    std::string name;
    std::string tableName;
    std::vector<FieldDef> fields;
    EventHandlers handlers;
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