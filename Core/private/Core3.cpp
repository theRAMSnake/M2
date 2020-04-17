#include "Core3.hpp"
#include ""

namespace materia
{

Core3::Core3(const CoreConfig& config)
: mDb(config.dbFileName)
, mTypeSystem(mDb)
, mOldCore(mDb, config.dbFileName)
{

}

IInbox& Core3::getInbox()
{
   return mOldCore.getInbox();
}

ICalendar& Core3::getCalendar()
{
   return mOldCore.getCalendar();
}

IStrategy_v2& Core3::getStrategy_v2()
{
   return mOldCore.getStrategy_v2();
}

IBackuper& Core3::getBackuper()
{
   return mOldCore.getBackuper();
}

IJournal& Core3::getJournal()
{
   return mOldCore.getJournal();
}

IFreeData& Core3::getFreeData()
{
   return mOldCore.getFreeData();
}

IFinance& Core3::getFinance()
{
   return mOldCore.getFinance();
}

IReward& Core3::getReward()
{
   return mOldCore.getReward();
}

IChallenge& Core3::getChallenge()
{
   return mOldCore.getChallenge();
}

void Core3::onNewDay()
{
   mOldCore.onNewDay();
}

void Core3::onNewWeek()
{
   mOldCore.onNewWeek();
}

std::shared_ptr<ICore3> createCore(const CoreConfig& config)
{
   return std::make_shared<Core3>(config);
}

std::string Core3::executeCommandJson(const std::string& json)
{
   try
   {
      auto cmd = parseCommand(json);

      return execute(cmd);
   }
   catch(std::exception ex)
   {
      return ErrorResponce(ex);
   }
}

}