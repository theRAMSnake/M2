#pragma once
#include "TypeSystem.hpp"
#include "Database.hpp"
#include "Expressions.hpp"
#include "TypeHandler.hpp"

namespace materia
{

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

    void initialize();

    Connections& getConnections();

private:
    TypeHandler& getHandler(const std::string& typeName);

    std::map<std::string, std::shared_ptr<TypeHandler>> mHandlers;
    Database& mDb;
    Connections& mConnections;
    TypeSystem& mTypes;
};

}
