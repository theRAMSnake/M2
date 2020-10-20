#pragma once
#include "TypeSystem.hpp"
#include "Object.hpp"
#include "Database.hpp"
#include "Expressions.hpp"
#include <set>
#include <future>

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
    TypeHandler(
        const TypeDef& type, 
        Database& db
        );

    //Id must be unique
    Object create(const std::optional<Id> id, const IValueProvider& provider);
    std::vector<Object> query(const std::vector<Id>& ids);
    std::vector<Object> query(const Filter& f);
    std::optional<Object> get(const Id& id);
    std::vector<Object> getAll();
    //Id must be present
    void destroy(const Id id);
    bool contains(const Id id);
    //Id must be present
    void modify(const Id id, const IValueProvider& provider);
    void modify(const Object& obj);

private:
    const TypeDef mType;
    std::unique_ptr<DatabaseTable> mStorage;
    
    std::map<Id, Object> mPool;
};

}