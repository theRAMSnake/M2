#pragma once
#include <string>
#include <map>
#include <memory>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "sqlite_modern_cpp.h"
#include <Common/Id.hpp>

namespace materia
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

class Database;
class DatabaseTable
{
public:

    DatabaseTable(const std::string& name, sqlite::database& sqdb, Database& db);

    //Assumes access only on startup
    template<class TFunctor>
    void foreach(TFunctor f)
    {
        mSqdb << "SELECT * FROM " + mName >> f;
    }

    void store(const Id& id, const std::string& data);
    void directStore(const Id& id, const std::string& data);
    void store(const std::map<Id, std::string>& data);
    void erase(const Id& id);
    std::optional<std::string> load(const Id& id);

private:
    const std::string mName;
    sqlite::database& mSqdb;
    Database& mDb;
    std::shared_ptr<BinderHolder> mInsertBinder;
    std::shared_ptr<BinderHolder> mEraseBinder;
};

class Database
{
public:
    friend class DatabaseTable;
    Database(const std::string& dbPath);

    std::unique_ptr<DatabaseTable> getTable(const std::string& name);

    ~Database();

private:
    struct QueueEntry
    {
        bool isStore;
        std::shared_ptr<BinderHolder> binder;
        Id id;
        std::string data;
    };

    void queueStore(std::shared_ptr<BinderHolder> binder, const Id& id, const std::string& data);
    void queueErase(std::shared_ptr<BinderHolder> binder, const Id& id);

    void threadFunc();

    bool mStop = false;
    sqlite::database mDb;
    std::thread mThread;
    std::mutex mMutex;
    std::condition_variable mCondVar;
    std::vector<QueueEntry> mQueue;
};

}
