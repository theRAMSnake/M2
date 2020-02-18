#include "Database.hpp"

namespace materia3
{

Database::Database(const std::string& dbPath)
: mDb(dbPath)
{
    mDb << "CREATE TABLE IF NOT EXISTS Items (Id text, Metadata text, Json text)";
    mInsertBinder.reset(new sqlite::database_binder(mDb << "REPLACE INTO Items VALUES (?, ?, ?);"));
    mEraseBinder.reset(new sqlite::database_binder(mDb << "DELETE FROM Items WHERE Id = ?;"));
}

void Database::store(const Id& id, const std::string& metadata, const std::string& data)
{
    auto& binder = (*mInsertBinder);
    binder << id << metadata << data;
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
    std::string metadata;
    std::string data;

    auto f = [&](std::string loadedId, std::string loadedMetadata, std::string loadedData){
        data = loadedData;
        metadata = loadedMetadata;
    };

    mDb << "SELECT * FROM Items WHERE Id = ?" << id.getGuid() >> f;

    return data.empty() ? std::unique_ptr<DatabaseSlot>() : std::make_unique<DatabaseSlot>(id, metadata, *this, data);
}

std::unique_ptr<DatabaseSlot> Database::allocate(const Id& id, const std::string& metadata)
{
    return std::make_unique<DatabaseSlot>(id, metadata, *this); 
}

Database::~Database()
{
}

DatabaseSlot::DatabaseSlot(const Id& id, const std::string& metadata, Database& db)
: mId(id)
, mDb(db)
, mMetadata(metadata)
{
    put("");
}

DatabaseSlot::DatabaseSlot(const Id& id, const std::string& metadata, Database& db, const std::string& data)
: mId(id)
, mDb(db)
, mData(data)
, mMetadata(metadata)
{

}

const std::string& DatabaseSlot::get() const
{
    return mData;
}

void DatabaseSlot::put(const std::string& data)
{
    mDb.store(mId, mMetadata, data);
}

void DatabaseSlot::clear()
{
    mDb.erase(mId);
}

const std::string& DatabaseSlot::getMetadata() const
{
    return mMetadata;
}

}