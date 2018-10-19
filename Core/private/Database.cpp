#include "Database.hpp"

namespace materia
{

Database::Database(const std::string& dbPath)
: mDb(dbPath)
{
    mDb << "CREATE TABLE IF NOT EXISTS Items (Id text, Json text)";
    mInsertBinder.reset(new sqlite::database_binder(mDb << "INSERT INTO Items VALUES (?, ?);"));
    mEraseBinder.reset(new sqlite::database_binder(mDb << "DELETE FROM Items WHERE Id = ?;"));
}

void Database::store(const std::map<Id, std::string>& data)
{
    auto& binder = (*mInsertBinder);
    for(auto x : data)
    {
        binder << x.first << x.second;
        binder++;
    }
}

void Database::store(const Id& id, const std::string& data)
{
    auto& binder = (*mInsertBinder);
    binder << id << data;
    binder++;
}

void Database::erase(const Id& id)
{
    auto& binder = (*mEraseBinder);
    binder << id;
    binder++;
}

Database::~Database()
{
}

}