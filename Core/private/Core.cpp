#include "Core.hpp"

namespace materia
{

Core::Core(Database& db, const std::string& dbFileName)
: mDb(db)
, mReward(mDb)
, mCalendar(mDb, mReward)
, mJournal(mDb)
, mChallenge(mDb, mReward)
, mStrategy_v2(mDb, mReward, mChallenge)
, mBackuper(dbFileName)
, mFinance(mDb)
{

}

ICalendar& Core::getCalendar()
{
   return mCalendar;
}

IStrategy_v2& Core::getStrategy_v2()
{
   return mStrategy_v2;
}

IBackuper& Core::getBackuper()
{
   return mBackuper;
}

IJournal& Core::getJournal()
{
   return mJournal;
}

IFinance& Core::getFinance()
{
   return mFinance;
}

IReward& Core::getReward()
{
   return mReward;
}

IChallenge& Core::getChallenge()
{
   return mChallenge;
}

void Core::onNewDay()
{
   
}

void Core::onNewWeek()
{
   mChallenge.resetWeekly();
}

}