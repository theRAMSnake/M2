#pragma once
#include "TraitSystem.hpp"
#include "Database.hpp"
#include "Expressions.hpp"

namespace materia
{

using Filter = Expression;

class ITypeHandler
{
public:
    virtual Id create(const Id id, const std::vector<std::string>& traits, const Params& params) = 0;
    virtual std::vector<Params> query(const std::vector<Id>& ids) = 0;
    virtual std::vector<Params> query(const Filter& f) = 0;
    virtual std::optional<Params> get(const Id& id) = 0;
    virtual bool destroy(const Id id) = 0;
    virtual void modify(const Id id, const Params& params) = 0;
};

class ObjectManager
{
public:
    ObjectManager(Database& db, TraitSystem& types);

    Id create(const std::vector<std::string>& traits, const Id id, const Params& params);
    void modify(const Id id, const Params& params);
    std::vector<Params> query(const std::vector<Id>& ids);
    std::vector<Params> query(const Filter& filter);
    void destroy(const Id id);

private:
    ITypeHandler& getHandler(const std::string& traitName);
    std::optional<std::tuple<ITypeHandler&, Params>> lookup(const Id id);

    std::map<std::string, std::shared_ptr<ITypeHandler>> mHandlers;
    std::shared_ptr<ITypeHandler> mDefaultHandler;
    Database& mDb;
    TraitSystem& mTypes;
};

}