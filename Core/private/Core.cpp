#include "Core.hpp"

namespace materia
{

Core::Core(Database& db, const std::string& dbFileName)
: mDb(db)
, mReward(mDb)
, mStrategy_v2(mDb, mReward)
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

IReward& Core::getReward()
{
   return mReward;
}

void Core::onNewDay()
{
   
}

void Core::onNewWeek()
{
   
}

}