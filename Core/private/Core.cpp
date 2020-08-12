#include "Core.hpp"

namespace materia
{

Core::Core(Database& db, const std::string& dbFileName)
: mDb(db)
, mStrategy_v2(mDb)
, mBackuper(dbFileName)
{

}

IStrategy_v2& Core::getStrategy_v2()
{
   return mStrategy_v2;
}

IBackuper& Core::getBackuper()
{
   return mBackuper;
}

void Core::onNewDay()
{
   
}

void Core::onNewWeek()
{
   
}

}