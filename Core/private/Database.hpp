#pragma once
#include <string>
#include <map>
#include "sqlite_modern_cpp.h"
#include "../Id.hpp"

namespace materia
{

//Thoughts: If slow - divide by tables
class Database
{
public:
    Database(const std::string& dbPath);

    template<class TFunctor>
    void foreach(TFunctor f)
    {
        mDb << "SELECT * FROM Items" >> f;
    }

    void store(const Id& id, const std::string& data);
    void store(const std::map<Id, std::string>& data);
    void erase(const Id& id);

    ~Database();

private:
    sqlite::database mDb;
    std::unique_ptr<sqlite::database_binder> mInsertBinder;
    std::unique_ptr<sqlite::database_binder> mEraseBinder;
};

}