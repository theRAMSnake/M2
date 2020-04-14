#include "Core.hpp"

namespace materia
{

Core::Core(Database& db, const std::string& dbFileName)
: mDb(db)
, mInbox(mDb)
, mReward(mDb)
, mCalendar(mDb, mReward)
, mJournal(mDb)
, mFreeData(mDb)
, mChallenge(mDb, mReward)
, mStrategy_v2(mDb, mFreeData, mReward, mChallenge)
, mBackuper(dbFileName)
, mFinance(mDb)
{

}

IInbox& Core::getInbox()
{
   return mInbox;
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

IFreeData& Core::getFreeData()
{
   return mFreeData;
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
   mFinance.performAnalisys(mReward);
}

void Core::onNewWeek()
{
   mChallenge.resetWeekly();
}

}