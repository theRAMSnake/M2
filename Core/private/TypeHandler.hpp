#pragma once
#include "TypeSystem.hpp"
#include "Object.hpp"
#include "Database.hpp"
#include "Expressions.hpp"
#include <set>

namespace materia
{

using Filter = Expression;

class IValueProvider
{
public:
    virtual void populate(Object& obj) const = 0;
    virtual ~IValueProvider(){}
};

class TypeHandler
{
public:
    TypeHandler(const TypeDef& type, Database& db, std::function<void(Object&)> onChangeHandler);

    //Id must be unique
    ObjectPtr create(const std::optional<Id> id, const IValueProvider& provider);
    std::vector<ObjectPtr> query(const std::vector<Id>& ids);
    std::vector<ObjectPtr> query(const Filter& f);
    std::optional<ObjectPtr> get(const Id& id);
    std::vector<ObjectPtr> getAll();
    //Id must be present
    void destroy(const Id id);
    bool contains(const Id id);
    //Id must be present
    void modify(const Id id, const IValueProvider& provider);
    void modify(const Object& obj);

private:
    const TypeDef mType;
    std::unique_ptr<DatabaseTable> mStorage;
    std::function<void(Object&)> mOnChangeHandler;

    std::set<Id> mIds;
};

}