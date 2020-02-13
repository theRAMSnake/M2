#include "Database.hpp"

namespace materia3
{

Database::Database(const std::string& dbPath)
: mDb(dbPath)
{
    mDb << "CREATE TABLE IF NOT EXISTS Items (Id text, Json text)";
    mInsertBinder.reset(new sqlite::database_binder(mDb << "REPLACE INTO Items VALUES (?, ?);"));
    mEraseBinder.reset(new sqlite::database_binder(mDb << "DELETE FROM Items WHERE Id = ?;"));
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

std::unique_ptr<DatabaseSlot> Database::load(const Id& id)
{
    std::string data;

    auto f = [&](std::string loadedId, std::string loadedData){
        data = loadedData;
    };

    mDb << "SELECT * FROM Items WHERE Id = ?" << id.getGuid() >> f;

    return data.empty() ? std::unique_ptr<DatabaseSlot>() : std::make_unique<DatabaseSlot>(id, *this, data);
}

std::unique_ptr<DatabaseSlot> Database::allocate(const Id& id)
{
    return std::make_unique<DatabaseSlot>(id, *this); 
}

Database::~Database()
{
}

DatabaseSlot::DatabaseSlot(const Id& id, Database& db)
: mId(id)
, mDb(db)
{
    put("");
}

DatabaseSlot::DatabaseSlot(const Id& id, Database& db, const std::string& data)
: mId(id)
, mDb(db)
, mData(data)
{

}

const std::string& DatabaseSlot::get() const
{
    return mData;
}

void DatabaseSlot::put(const std::string& data)
{
    mDb.store(mId, data);
}

void DatabaseSlot::clear()
{
    mDb.erase(mId);
}

}