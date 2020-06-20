#pragma once
#include "TypeSystem.hpp"
#include "Database.hpp"
#include "Expressions.hpp"
#include "TypeHandler.hpp"
#include "../IReward.hpp"

namespace materia
{

class ObjectManager
{
public:
    ObjectManager(Database& db, TypeSystem& types, IReward& reward);

    ObjectPtr create(const std::optional<Id> id, const std::string& type, const IValueProvider& provider);
    void modify(const Id id, const IValueProvider& provider);
    void modify(const Object& obj);
    std::vector<ObjectPtr> query(const std::vector<Id>& ids);
    std::vector<ObjectPtr> query(const Filter& f);
    void destroy(const Id id);
    std::vector<ObjectPtr> describe() const;
    ObjectPtr get(const Id id);
    ObjectPtr getOrCreate(const Id id, const std::string& type);
    std::vector<ObjectPtr> getAll(const std::string& type);

    IReward& LEGACY_getReward();

private:
    TypeHandler& getHandler(const std::string& typeName);

    std::function<void(Object&)> createOnChangeHandler(const std::string& typeName);
    void handleChItemChange(Object& obj);

    std::map<std::string, std::shared_ptr<TypeHandler>> mHandlers;
    Database& mDb;
    TypeSystem& mTypes;
    IReward& mReward;
};

}