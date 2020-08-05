#include "Core3.hpp"
#include "JsonSerializer.hpp"
#include "JsonRestorationProvider.hpp"
#include "types/Variable.hpp"
#include "types/SimpleList.hpp"
#include <chrono>

#include "subsystems/User.hpp"
#include "subsystems/Finance.hpp"
#include "subsystems/Challenge.hpp"
#include "subsystems/Calendar.hpp"
#include "subsystems/Common.hpp"
#include "subsystems/Journal.hpp"
#include "subsystems/Reward.hpp"

namespace materia
{

Core3::Core3(const CoreConfig& config)
: mDb(config.dbFileName)
, mOldCore(mDb, config.dbFileName)
, mObjManager(mDb, mTypeSystem, mOldCore.getReward())
{
   mSubsystems.push_back(std::make_shared<ChallengeSS>(mObjManager));
   mSubsystems.push_back(std::make_shared<FinanceSS>(mObjManager));
   mSubsystems.push_back(std::make_shared<UserSS>(mObjManager));
   mSubsystems.push_back(std::make_shared<CommonSS>(mObjManager));
   mSubsystems.push_back(std::make_shared<JournalSS>(mObjManager));
   mSubsystems.push_back(std::make_shared<CalendarSS>(mObjManager));
   mSubsystems.push_back(std::make_shared<RewardSS>(mObjManager));

   for(auto s : mSubsystems)
   {
      for(auto t : s->getTypes())
      {
         mTypeSystem.add(t);
      }
   }

   mObjManager.initialize();
}

IStrategy_v2& Core3::getStrategy_v2()
{
   return mOldCore.getStrategy_v2();
}

IBackuper& Core3::getBackuper()
{
   return mOldCore.getBackuper();
}

IReward& Core3::getReward()
{
   return mOldCore.getReward();
}

void Core3::onNewDay()
{
   for(auto s : mSubsystems)
   {
      s->onNewDay();
   }
}

void Core3::onNewWeek()
{
   for(auto s : mSubsystems)
   {
      s->onNewWeek();
   }
}

std::shared_ptr<ICore3> createCore(const CoreConfig& config)
{
   return std::make_shared<Core3>(config);
}

std::string Core3::formatResponce(const ExecutionResult& result)
{
   Object responce(*mTypeSystem.get("object"), Id::generate());

   if(std::holds_alternative<Success>(result))
   {
      responce["success"] = true;
   }
   else if(std::holds_alternative<ObjectList>(result))
   {
      auto& objList = std::get<ObjectList>(result);
      responce.setChildren("object_list", objList);
   }
   else if(std::holds_alternative<std::string>(result))
   {
      responce["result"] = std::get<std::string>(result);
   }
   else if(std::holds_alternative<Id>(result))
   {
      responce["result_id"] = std::get<Id>(result);
   }
   else
   {
      throw std::runtime_error("Cannot format responce");
   }

   return responce.toJson();
}

std::string Core3::formatErrorResponce(const std::string& errorText)
{
   Object responce(*mTypeSystem.get("object"), Id::Invalid);

   responce["error"] = errorText;

   return responce.toJson();
}

std::string Core3::executeCommandJson(const std::string& json)
{
   std::chrono::time_point<std::chrono::high_resolution_clock> started = std::chrono::high_resolution_clock::now();
   std::string result;

   try
   {
      auto cmd = parseCommand(json);

      result = formatResponce(cmd->execute(mObjManager));
   }
   catch(std::exception& ex)
   {
      result = formatErrorResponce(ex.what());
   }
   catch(...)
   {
      result = formatErrorResponce("Uknown error");
   }

   auto time_D_msec = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - started);

   if(time_D_msec > std::chrono::milliseconds(250))
   {
      notifyLongCommand(json, time_D_msec.count());
   }

   return result;
}

void Core3::notifyLongCommand(const std::string& cmd, unsigned int value)
{
   types::SimpleList inbox(mObjManager, Id("inbox"));
   inbox.add("Command execution exceeds time limit: (" + std::to_string(value) + "ms) " + cmd);
}

}