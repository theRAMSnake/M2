#pragma once
#include "TypeSystem.hpp"
#include "Database.hpp"
#include <boost/property_tree/ptree.hpp>

using Params = boost::property_tree::ptree;

namespace materia
{

class ITypeHandler
{
public:
    virtual Id create(const Params& params) = 0;
};

class ObjectManager
{
public:
    ObjectManager(Database& db, TypeSystem& types);

    Id create(const TypeDef& type, const Params& params);

private:
    void onNewTypeAdded(const TypeDef type);

    std::map<std::pair<std::string, std::string>, std::shared_ptr<ITypeHandler>> mHandlers;
    Database& mDb;
    TypeSystem& mTypes;
};

}