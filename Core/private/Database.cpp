#include "Database.hpp"
#include "Logger.hpp"

namespace materia
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

DatabaseTable::DatabaseTable(const std::string& name, sqlite::database& sqdb, Database& db)
: mName(name)
, mSqdb(sqdb)
, mDb(db)
{
    sqdb << "CREATE TABLE IF NOT EXISTS " + mName + " (Id text, Json text, PRIMARY KEY (Id))";
    mInsertBinder.reset(new BinderHolder(std::make_unique<sqlite::database_binder>(sqdb << "REPLACE INTO " + mName + " VALUES (?, ?);")));
    mEraseBinder.reset(new BinderHolder(std::make_unique<sqlite::database_binder>(sqdb << "DELETE FROM " + mName + " WHERE Id = ?;")));
}

Database::Database(const std::string& dbPath)
: mDb(dbPath)
, mThread(std::bind(&Database::threadFunc, this))
{
    
}

std::unique_ptr<DatabaseTable> Database::getTable(const std::string& name)
{
    return std::unique_ptr<DatabaseTable>(new DatabaseTable(name, mDb, *this));
}

void DatabaseTable::store(const Id& id, const std::string& data)
{
    mDb.queueStore(mInsertBinder, id, data);
}

std::optional<std::string> DatabaseTable::load(const Id& id)
{
    std::optional<std::string> result;

    mSqdb << "SELECT Json FROM " + mName + " WHERE Id = ?" << id.getGuid() >> [&](std::string field)
    {
        result = field;
    };

    return result;
}

void DatabaseTable::erase(const Id& id)
{
    mDb.queueErase(mEraseBinder, id);
}

Database::~Database()
{
    mStop = true;
    mCondVar.notify_one();
    mThread.join();
}

void Database::threadFunc()
{
    while(true)
    {
        std::vector<QueueEntry> newEntries;

        {
            std::unique_lock<std::mutex> lock(mMutex);

            if(mQueue.empty())
            {
                if(mStop)
                {
                    return;
                }
                mCondVar.wait(lock);
            }

            newEntries = mQueue;
            mQueue.clear();
        }
        
        for(auto& x : newEntries)
        {
            if(x.isStore)
            {
                LOG("Operation is executed for " + x.id.getGuid());
                x.binder->get() << x.id << x.data;
            }
            else
            {
                x.binder->get() << x.id;
            }

            x.binder->get()++;
        }
    }
}

void Database::queueStore(std::shared_ptr<BinderHolder> binder, const Id& id, const std::string& data)
{
    std::unique_lock<std::mutex> lock(mMutex);
    mQueue.push_back({true, binder, id, data});
    mCondVar.notify_one();
}

void Database::queueErase(std::shared_ptr<BinderHolder> binder, const Id& id)
{
    std::unique_lock<std::mutex> lock(mMutex);
    mQueue.push_back({false, binder, id});
    mCondVar.notify_one();
}

}
