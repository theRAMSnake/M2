#include "Core.hpp"

namespace materia
{

Core::Core(const CoreConfig& config)
: mDb(config.dbFileName)
, mInbox(mDb)
, mCalendar(mDb)
, mJournal(mDb)
, mStrategy(mDb)
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

IJournal& Core::getJournal()
{
   return mJournal;
}

std::shared_ptr<ICore> createCore(const CoreConfig& config)
{
   return std::shared_ptr<ICore>(new Core(config));
}

}