#pragma once
#include "TypeSystem.hpp"
#include "Database.hpp"
#include "Expressions.hpp"

namespace materia
{

class ITypeHandler
{
public:
    virtual Id create(const Params& params) = 0;
    virtual void modify(const Params& params) = 0;
    virtual std::vector<Params> queryAll() = 0;
    virtual void destroy(const Id id) = 0;
};

using Filter = Expression;

class ObjectManager
{
public:
    ObjectManager(Database& db, TypeSystem& types);

    Id create(const TypeDef& type, const Params& params);
    void modify(const TypeDef& type, const Params& params);
    std::vector<Params> query(const TypeDef& type);
    std::vector<Params> query(const TypeDef& type, const Filter& filter);
    void destroy(const TypeDef& type, const Id id);

private:
    void onNewTypeAdded(const TypeDef type);

    std::map<std::pair<std::string, std::string>, std::shared_ptr<ITypeHandler>> mHandlers;
    Database& mDb;
    TypeSystem& mTypes;
};

}