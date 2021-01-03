#include "Core3.hpp"
#include "JsonSerializer.hpp"
#include "JsonRestorationProvider.hpp"
#include "ExceptionsUtil.hpp"
#include "types/Variable.hpp"
#include "types/SimpleList.hpp"
#include "rng.hpp"
#include <chrono>
#include <random>

#include "subsystems/User.hpp"
#include "subsystems/Finance.hpp"
#include "subsystems/Calendar.hpp"
#include "subsystems/Common.hpp"
#include "subsystems/Journal.hpp"
#include "subsystems/Reward.hpp"
#include "subsystems/Strategy.hpp"

namespace materia
{

Command* parseCreate(const boost::property_tree::ptree& src)
{
   auto typeName = getOrThrow<std::string>(src, "typename", "Typename is not specified");
   std::optional<Id> stdid;
   auto id = src.get_optional<std::string>("defined_id");
   if(id)
   {
       stdid = *id;
   }
   auto params = parseParams(src);

   return new CreateCommand(stdid, typeName, writeJson(params));
}

Command* parseQuery(const boost::property_tree::ptree& src)
{
   auto ids = parseIds(src);
   auto filter = parseFilter(src);

   return new QueryCommand(filter, ids);
}

Command* parseDestroy(const boost::property_tree::ptree& src)
{
   auto id = getOrThrow<Id>(src, "id", "Id is not specified");

   return new DestroyCommand(id);
}

Command* parseModify(const boost::property_tree::ptree& src)
{
   auto id = getOrThrow<Id>(src, "id", "Id is not specified");
   auto params = parseParams(src);

   return new ModifyCommand(id, writeJson(params));
}

Command* parseDescribe(const boost::property_tree::ptree& src)
{
   return new DescribeCommand();
}

Command* parseBackup(const boost::property_tree::ptree& src, const std::string dbFileName)
{
   return new BackupCommand(dbFileName);
}

Core3::Core3(const CoreConfig& config)
: mDb(config.dbFileName)
, mObjManager(mDb, mTypeSystem)
{
   auto rewardSS = std::make_shared<RewardSS>(mObjManager);
   mSubsystems.push_back(rewardSS);

   auto commonSS = std::make_shared<CommonSS>(mObjManager);
   mSubsystems.push_back(commonSS);
   mCommonSS = commonSS.get();

   auto strategySS = std::make_shared<StrategySS>(mObjManager, *rewardSS);
   mSubsystems.push_back(strategySS);

   mSubsystems.push_back(std::make_shared<FinanceSS>(mObjManager, *rewardSS, *commonSS));
   mSubsystems.push_back(std::make_shared<UserSS>(mObjManager, *rewardSS, *strategySS));
   mSubsystems.push_back(std::make_shared<JournalSS>(mObjManager));
   mSubsystems.push_back(std::make_shared<CalendarSS>(mObjManager));
  
   for(auto s : mSubsystems)
   {
      for(auto t : s->getTypes())
      {
         mTypeSystem.add(t);
      }

      for(auto d : s->getCommandDefs())
      {
         mCommandDefs.push_back(d);
      }
   }

   mCommandDefs.push_back({"create", parseCreate});
   mCommandDefs.push_back({"query", parseQuery});
   mCommandDefs.push_back({"modify", parseModify});
   mCommandDefs.push_back({"destroy", parseDestroy});
   mCommandDefs.push_back({"describe", parseDescribe});
   mCommandDefs.push_back({"backup", std::bind(parseBackup, std::placeholders::_1, config.dbFileName)});

   mObjManager.initialize();
}

void Core3::onNewDay(const boost::gregorian::date& date)
{
   for(auto s : mSubsystems)
   {
      s->onNewDay(date);
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

   bool longCommand = false;

   try
   {
      auto cmd = parseCommand(json, mCommandDefs);

      longCommand = cmd->isLong();

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

   if(!longCommand && time_D_msec > std::chrono::milliseconds(250))
   {
      notifyLongCommand(json, time_D_msec.count());
   }

   return result;
}

void Core3::notifyLongCommand(const std::string& cmd, unsigned int value)
{
   mCommonSS->push(Id("inbox"), "Command execution exceeds time limit: (" + std::to_string(value) + "ms) " + cmd);
}

}