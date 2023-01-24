#pragma once
#include "TypeSystem.hpp"
#include "Database.hpp"
#include "Object.hpp"
#include "Expressions2.hpp"
#include <unordered_map>

namespace materia
{

using Filter = v2::Expression;
class IValueProvider
{
public:
    virtual void populate(Object& obj) const = 0;
    virtual ~IValueProvider(){}
};

class Connections;
class ObjectManager
{
public:
    ObjectManager(Database& db, TypeSystem& types, Connections& connections);

    Object create(const std::optional<Id> id, const std::string& type, const IValueProvider& provider);
    void modify(const Id id, const IValueProvider& provider);
    void modify(const Object& obj);
    std::vector<Object> query(const std::vector<Id>& ids);
    std::vector<Object> query(const Filter& f);
    void destroy(const Id id);
    std::vector<Object> describe() const;
    Object get(const Id id);
    Object getOrCreate(const Id id, const std::string& type);
    std::vector<Object> getAll(const std::string& type);

    void initialize(Database& db);

    Connections& getConnections();

private:

    std::map<std::string, std::unique_ptr<DatabaseTable>> mStorages;
    std::unordered_map<Id, Object> mPool;

    Connections& mConnections;
    TypeSystem& mTypes;
};

}
