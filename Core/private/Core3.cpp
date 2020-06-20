#include "Core3.hpp"
#include "JsonSerializer.hpp"
#include "Finance.hpp"
#include "types/Variable.hpp"

namespace materia
{

Core3::Core3(const CoreConfig& config)
: mDb(config.dbFileName)
, mTypeSystem()
, mOldCore(mDb, config.dbFileName)
, mObjManager(mDb, mTypeSystem, mOldCore.getReward())
{

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
   //Inbox award
   types::SimpleList inbox(mObjManager, Id("inbox"));
   if(inbox.size() == 0 && rand() % 10 == 0)
   {
      getReward().addPoints(1);
      inbox.add("Extra point awarded for empty inbox.");
   }

   //Finance analisys
   static_cast<Finance&>(getFinance()).performAnalisys(getReward(), inbox);

   //TOD reselection
   generateNewTOD();
}

void Core3::generateNewTOD()
{
   types::SimpleList wisdom(mObjManager, Id("wisdom"));
   types::Variable tod(mObjManager, Id("tip_of_the_day"));
   if(wisdom.size() > 0)
   {
      auto pos = rand() % wisdom.size();
      tod = wisdom.at(pos);
   }
}

void Core3::onNewWeek()
{
   mOldCore.onNewWeek();
}

std::shared_ptr<ICore3> createCore(const CoreConfig& config)
{
   return std::make_shared<Core3>(config);
}

std::string Core3::formatResponce(const ExecutionResult& result)
{
   Object responce(*mTypeSystem.get("object"), Id::Invalid);

   if(std::holds_alternative<Success>(result))
   {
      responce["success"] = true;
   }
   else if(std::holds_alternative<ObjectList>(result))
   {
      responce["object_list"] = std::get<ObjectList>(result);
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