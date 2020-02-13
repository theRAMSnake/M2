#pragma once
#include <string>
#include <map>
#include "sqlite_modern_cpp.h"
#include "Common/Id.hpp"

namespace materia3
{

using Id = materia::Id;

class Database;
class DatabaseSlot
{
public:
    DatabaseSlot(const Id& id, Database& db);
    DatabaseSlot(const Id& id, Database& db, const std::string& data);

    const std::string& get() const;
    void put(const std::string& data);
    void clear();

private:
    const Id mId;
    Database& mDb;
    std::string mData;
};

class Database
{
public:
    friend class DatabaseSlot;

    Database(const std::string& dbPath);

    std::unique_ptr<DatabaseSlot> load(const Id& id);
    std::unique_ptr<DatabaseSlot> allocate(const Id& id);

    ~Database();

protected:
    void store(const Id& id, const std::string& data);
    void erase(const Id& id);

private:
    sqlite::database mDb;
    std::unique_ptr<sqlite::database_binder> mInsertBinder;
    std::unique_ptr<sqlite::database_binder> mEraseBinder;
};

}