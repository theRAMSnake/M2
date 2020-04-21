#include "Core3.hpp"
#include "CommandParser.hpp"
#include "JsonSerializer.hpp"

namespace materia
{

Core3::Core3(const CoreConfig& config)
: mDb(config.dbFileName)
, mTypeSystem(mDb)
, mObjManager(mDb, mTypeSystem)
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

std::string formatResponce(const ExecutionResult& result)
{
   boost::property_tree::ptree responce;

   if(std::holds_alternative<Success>(result))
   {
      responce.put("success", "");
   }
   else if(std::holds_alternative<ObjectList>(result))
   {
      auto& ol = std::get<ObjectList>(result);
      Params subParams;
      for(auto& o: ol)
      {
         subParams.push_back({"", o});
      }

      responce.add_child("object_list", subParams);
   }
   else if(std::holds_alternative<std::string>(result))
   {
      responce.put("result", "");
   }
   else if(std::holds_alternative<Id>(result))
   {
      responce.put("id", std::get<Id>(result));
   }
   else
   {
      throw std::runtime_error("Cannot format responce");
   }

   return writeJson(responce);
}

std::string formatErrorResponce(const std::string& errorText)
{
   boost::property_tree::ptree err;
   err.put("error", errorText);

   return writeJson(err);
}

std::string Core3::executeCommandJson(const std::string& json)
{
   try
   {
      auto cmd = parseCommand(json);

      return formatResponce(cmd->execute(mObjManager));
   }
   catch(std::exception& ex)
   {
      return formatErrorResponce(ex.what());
   }
   catch(...)
   {
      return formatErrorResponce("Uknown error");
   }
}

}