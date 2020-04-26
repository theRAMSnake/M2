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

struct TraitDef
{
    std::string name;
    bool isCoreTrait;
};

boost::property_tree::ptree toPropertyTree(const TraitDef t);

class Database;
class TraitSystem
{
public:
    boost::signals2::signal<void(const TraitDef)> onAdded;

    TraitSystem(Database& db);

    std::optional<TraitDef> get(const std::string& name) const;
    std::optional<TraitDef> get(const Id id) const;
    std::vector<TraitDef> get() const;

    Id add(const TraitDef& newType);
    bool remove(const std::string& name);
    bool remove(const Id id);

private:
    std::unique_ptr<DatabaseTable> mStorage;
};

}