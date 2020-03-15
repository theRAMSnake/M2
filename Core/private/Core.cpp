#include "Core.hpp"

namespace materia
{

Core::Core(const CoreConfig& config)
: mDb(config.dbFileName)
, mInbox(mDb)
, mReward(mDb)
, mCalendar(mDb, mReward)
, mJournal(mDb)
, mFreeData(mDb)
, mStrategy_v2(mDb, mFreeData)
, mBackuper(config.dbFileName)
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

std::shared_ptr<ICore> createCore(const CoreConfig& config)
{
   return std::shared_ptr<ICore>(new Core(config));
}

}