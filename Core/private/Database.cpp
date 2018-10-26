#include "Database.hpp"

namespace materia
{

DatabaseTable::DatabaseTable(const std::string& name, sqlite::database& db)
: mName(name)
, mDb(db)
{
    mDb << "CREATE TABLE IF NOT EXISTS " + mName + " (Id text, Json text, PRIMARY KEY (Id))";
    mInsertBinder.reset(new sqlite::database_binder(mDb << "REPLACE INTO " + mName + " VALUES (?, ?);"));
    mEraseBinder.reset(new sqlite::database_binder(mDb << "DELETE FROM " + mName + " WHERE Id = ?;"));
}

Database::Database(const std::string& dbPath)
: mDb(dbPath)
{
    
}

std::unique_ptr<DatabaseTable> Database::getTable(const std::string& name)
{
    return std::unique_ptr<DatabaseTable>(new DatabaseTable(name, mDb));
}

void DatabaseTable::store(const Id& id, const std::string& data)
{
    auto& binder = (*mInsertBinder);
    binder << id << data;
    binder++;
}

void DatabaseTable::erase(const Id& id)
{
    auto& binder = (*mEraseBinder);
    binder << id;
    binder++;
}

Database::~Database()
{
}

}