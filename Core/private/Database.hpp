#pragma once
#include <string>
#include <map>
#include <memory>
#include "sqlite_modern_cpp.h"
#include <Common/Id.hpp>

namespace materia
{

class DatabaseTable
{
public:

    DatabaseTable(const std::string& name, sqlite::database& db);

    template<class TFunctor>
    void foreach(TFunctor f)
    {
        mDb << "SELECT * FROM " + mName >> f;
    }

    void store(const Id& id, const std::string& data);
    void store(const std::map<Id, std::string>& data);
    void erase(const Id& id);

private:
    const std::string mName;
    sqlite::database& mDb;
    std::unique_ptr<sqlite::database_binder> mInsertBinder;
    std::unique_ptr<sqlite::database_binder> mEraseBinder;
};

class Database
{
public:
    Database(const std::string& dbPath);

    std::unique_ptr<DatabaseTable> getTable(const std::string& name);

    ~Database();

private:
    sqlite::database mDb;
    
};

}