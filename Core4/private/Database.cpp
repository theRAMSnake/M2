#include "Database.hpp"

namespace materia
{
namespace v4
{

BinderHolder::BinderHolder(std::unique_ptr<sqlite::database_binder>&& binder)
: mBinder(std::move(binder))
{

}

BinderHolder::~BinderHolder()
{
    mBinder->used(true);
}

sqlite::database_binder& BinderHolder::get()
{
    return *mBinder;
}

ObjectContentTable::ObjectContentTable(sqlite::database& sqdb)
    : DatabaseTable("m4_content_table", sqdb)
{
    sqdb << "CREATE TABLE IF NOT EXISTS m4_content_table(Id text, Json text, PRIMARY KEY(Id))";
    mInsertBinder.reset(new BinderHolder(std::make_unique<sqlite::database_binder>(sqdb << "REPLACE INTO m4_content_table VALUES (?, ?);")));
    mEraseBinder.reset(new BinderHolder(std::make_unique<sqlite::database_binder>(sqdb << "DELETE FROM m4_content_table WHERE Id = ?;")));
}

void ObjectContentTable::store(const Id& id, const std::string& data)
{
    mInsertBinder->get() << id << data;
    mInsertBinder->get()++;
}

void ObjectContentTable::erase(const Id& id)
{
    mEraseBinder->get() << id;
    mEraseBinder->get()++;
}

ObjectRelationTable::ObjectRelationTable(sqlite::database& sqdb)
    : DatabaseTable("m4_relation_table", sqdb)
{
    sqdb << "CREATE TABLE IF NOT EXISTS m4_relation_table(parent text, child text)";
    mInsertBinder.reset(new BinderHolder(std::make_unique<sqlite::database_binder>(sqdb << "REPLACE INTO m4_relation_table VALUES (?, ?);")));
    mEraseBinder.reset(new BinderHolder(std::make_unique<sqlite::database_binder>(sqdb << "DELETE FROM m4_relation_table WHERE parent = ? OR child = ?;")));
}

void ObjectRelationTable::store(const Id& parent, const Id& child)
{
    mInsertBinder->get() << parent << child;
    mInsertBinder->get()++;
}

void ObjectRelationTable::erase(const Id& id)
{
    mEraseBinder->get() << id << id;
    mEraseBinder->get()++;
}

DatabaseTable::DatabaseTable(const std::string& name, sqlite::database& sqdb)
: mName(name)
, mSqdb(sqdb)
{
}

Database::Database(const std::string& dbPath)
: mDb(dbPath)
, mRelationTable(mDb)
, mContentTable(mDb)
{

}

ObjectRelationTable& Database::getRelationTable()
{
    return mRelationTable;
}

ObjectContentTable& Database::getContentTable()
{
    return mContentTable;
}

}
}
