#pragma once
#include "TypeSystem.hpp"
#include "Database.hpp"
#include "Expressions.hpp"
#include "TypeHandler.hpp"

namespace materia
{

class ObjectManager
{
public:
    ObjectManager(Database& db, TypeSystem& types);

    ObjectPtr create(const std::optional<Id> id, const std::string& type, const IValueProvider& provider);
    void modify(const Id id, const IValueProvider& provider);
    std::vector<ObjectPtr> query(const std::vector<Id>& ids);
    std::vector<ObjectPtr> query(const Filter& f);
    void destroy(const Id id);
    std::vector<ObjectPtr> describe() const;

private:
    TypeHandler& getHandler(const std::string& typeName);

    std::map<std::string, std::shared_ptr<TypeHandler>> mHandlers;
    Database& mDb;
    TypeSystem& mTypes;
};

}