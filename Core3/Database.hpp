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
    DatabaseSlot(const Id& id, const std::string& metadata, Database& db);
    DatabaseSlot(const Id& id, const std::string& metadata, Database& db, const std::string& data);

    const std::string& get() const;
    void put(const std::string& data);
    void clear();

    const std::string& getMetadata() const;

private:
    const Id mId;
    Database& mDb;
    std::string mData;
    std::string mMetadata;
};

class Database
{
public:
    friend class DatabaseSlot;

    Database(const std::string& dbPath);

    std::unique_ptr<DatabaseSlot> load(const Id& id);
    std::unique_ptr<DatabaseSlot> allocate(const Id& id, const std::string& metadata);

    ~Database();

protected:
    void store(const Id& id, const std::string& metadata, const std::string& data);
    void erase(const Id& id);

private:
    sqlite::database mDb;
    std::unique_ptr<sqlite::database_binder> mInsertBinder;
    std::unique_ptr<sqlite::database_binder> mEraseBinder;
};

}