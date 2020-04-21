#pragma once
#include <string>
#include <optional>
#include <vector>
#include <memory>
#include "Database.hpp"
#include <boost/signals2.hpp>
#include <boost/property_tree/ptree.hpp>

namespace materia
{

struct TypeDef
{
    std::string domain;
    std::string name;
    bool isCoreType;
};

boost::property_tree::ptree toPropertyTree(const TypeDef t);

class Database;
class TypeSystem
{
public:
    boost::signals2::signal<void(const TypeDef)> onAdded;

    TypeSystem(Database& db);

    std::optional<TypeDef> get(const std::string& domain, const std::string& name) const;
    std::vector<TypeDef> get() const;

    Id add(const TypeDef& newType);
    void remove(const std::string& domain, const std::string& name);
    void remove(const Id id);

private:
    std::unique_ptr<DatabaseTable> mStorage;
};

}