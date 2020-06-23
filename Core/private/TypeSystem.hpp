#pragma once
#include <string>
#include <optional>
#include <vector>
#include <ctime>

namespace materia
{

enum class Type
{
    Int,
    Double,
    String,
    Bool,
    Array,
    Timestamp,
    Option,
    Money,
    Reference
};

struct Time
{
    std::time_t value;
};

struct FieldDef
{
    std::string name;
    Type type;
    std::vector<std::string> options;
    std::string refType;
};

struct TypeDef
{
    std::string name;
    std::string tableName;
    std::vector<FieldDef> fields;
};

class TypeSystem
{
public:
    TypeSystem();

    std::optional<TypeDef> get(const std::string& name) const;
    std::vector<TypeDef> get() const;

    void add(const TypeDef& newType);

private:
    std::vector<TypeDef> mTypes;
};

}