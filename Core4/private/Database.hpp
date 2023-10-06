#pragma once
#include <string>
#include <memory>
#include <vector>
#include "sqlite_modern_cpp.h"
#include <Common/Id.hpp>

namespace materia
{
namespace v4
{

class BinderHolder
{
public:
    BinderHolder(std::unique_ptr<sqlite::database_binder>&& binder);
    ~BinderHolder();

    sqlite::database_binder& get();

private:
    std::unique_ptr<sqlite::database_binder> mBinder;
};

class DatabaseTable
{
public:
    DatabaseTable(const std::string& name, sqlite::database& sqdb);

    template<class TFunctor>
    void readWholeTable(TFunctor f)
    {
        mSqdb << "SELECT * FROM " + mName >> f;
    }

protected:
    const std::string mName;
    sqlite::database& mSqdb;
};

class ObjectContentTable : public DatabaseTable
{
public:
    explicit ObjectContentTable(sqlite::database& sqdb);
    void store(const Id& id, const std::string& data);
    void erase(const Id& id);

private:
    std::shared_ptr<BinderHolder> mInsertBinder;
    std::shared_ptr<BinderHolder> mEraseBinder;
};

class ObjectRelationTable : public DatabaseTable
{
public:
    explicit ObjectRelationTable(sqlite::database& sqdb);
    void store(const Id& parent, const Id& child);
    void erase(const Id& id);

private:
    std::shared_ptr<BinderHolder> mInsertBinder;
    std::shared_ptr<BinderHolder> mEraseBinder;
};

class Database
{
public:
    friend class DatabaseTable;
    Database(const std::string& dbPath);

    ObjectRelationTable& getRelationTable();
    ObjectContentTable& getContentTable();

private:
    sqlite::database mDb;
    ObjectRelationTable mRelationTable;
    ObjectContentTable mContentTable;
};

}
}
