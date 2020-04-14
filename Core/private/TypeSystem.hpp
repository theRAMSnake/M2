#pragma once
#include <string>
#include <optional>
#include <vector>
#include <memory>
#include "Database.hpp"

namespace materia
{

struct TypeDef
{
    std::string domain;
    std::string name;
    bool isCoreType;
};

class Database;
class TypeSystem
{
public:
    TypeSystem(Database& db);

    std::optional<TypeDef> get(const std::string& domain, const std::string& name) const;
    std::vector<TypeDef> get() const;

    void add(const TypeDef& newType);
    void remove(const std::string& domain, const std::string& name);

private:
    std::unique_ptr<DatabaseTable> mStorage;
};

}