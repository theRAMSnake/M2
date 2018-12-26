#include "Core.hpp"

namespace materia
{

Core::Core(const CoreConfig& config)
: mDb(config.dbFileName)
, mInbox(mDb)
, mCalendar(mDb)
, mJournal(mDb)
, mStrategy(mDb)
, mBackuper(config.dbFileName)
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

IStrategy& Core::getStrategy()
{
   return mStrategy;
}

IBackuper& Core::getBackuper()
{
   return mBackuper;
}

IJournal& Core::getJournal()
{
   return mJournal;
}

std::shared_ptr<ICore> createCore(const CoreConfig& config)
{
   return std::shared_ptr<ICore>(new Core(config));
}

}