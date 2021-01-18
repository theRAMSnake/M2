#include "User.hpp"
#include "../ObjectManager.hpp"
#include "Strategy.hpp"
#include "../types/SimpleList.hpp"
#include "../types/Variable.hpp"
#include "../ExceptionsUtil.hpp"
#include "../rng.hpp"
#include <boost/date_time/gregorian/greg_date.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace materia
{

Time advance(const Time src, const std::string recType)
{
    //"Weekly", "Monthly", "Quarterly", "Yearly", "Bi-daily"
    Time result = src;

    if(recType == "Weekly")
    {
        result.value += 604800;
    }
    else if(recType == "Monthly")
    {
        result.value = boost::posix_time::to_time_t(boost::posix_time::from_time_t(result.value) + boost::gregorian::months(1));
    }
    else if(recType == "Quarterly")
    {
        result.value = boost::posix_time::to_time_t(boost::posix_time::from_time_t(result.value) + boost::gregorian::months(3));
    }
    else if(recType == "Yearly")
    {
        result.value = boost::posix_time::to_time_t(boost::posix_time::from_time_t(result.value) + boost::gregorian::months(12));
    }
    else if(recType == "Bi-daily")
    {    
        result.value += 172800;
    }

    return result;
}

void complete(const Id id, ObjectManager& om, RewardSS& reward, StrategySS& strategy)
{
    auto object = om.get(id);

    if(object.getType().name != "calendar_item")
    {
        throw std::runtime_error(fmt::format("Type {} is not completable", object.getType().name));
    }

    auto eType = object["entityTypeChoice"].get<Type::Choice>();
    auto recType = object["reccurencyTypeChoice"].get<Type::Choice>();
    if(eType == "Task")
    {
        reward.addPoints(1);
    }
    else if(eType == "StrategyNodeReference")
    {
        strategy.onCalendarReferenceCompleted(object["nodeReference"].toId());
    }

    if(recType != "None")
    {
        object["timestamp"] = advance(object["timestamp"].get<Type::Timestamp>(), recType);
        om.modify(object);
    }
    else
    {
        om.destroy(id);
    }
}

class CompleteCommand : public Command
{
public:
   CompleteCommand(const Id& id, RewardSS& reward, StrategySS& strategy)
   : mId(id)
   , mReward(reward)
   , mStrategy(strategy)
   {

   }

   ExecutionResult execute(ObjectManager& objManager) override
   {
      complete(mId, objManager, mReward, mStrategy);
      return Success{};
   }

private:
    const Id mId;
    RewardSS& mReward;
    StrategySS& mStrategy;
};

Command* UserSS::parseComplete(const boost::property_tree::ptree& src)
{
   auto id = getOrThrow<Id>(src, "id", "Id is not specified");

   return new CompleteCommand(id, mReward, mStrategy);
}

UserSS::UserSS(ObjectManager& objMan, RewardSS& reward, StrategySS& strategy)
: mOm(objMan)
, mReward(reward)
, mStrategy(strategy)
{

}

void UserSS::generateNewTOD()
{
   types::SimpleList wisdom(mOm, Id("wisdom"));
   types::Variable tod(mOm, Id("tip_of_the_day"));
   if(wisdom.size() > 0)
   {
      auto pos = Rng::gen32() % wisdom.size();
      tod = wisdom.at(pos);
   }
}

void UserSS::awardInbox()
{
   types::SimpleList inbox(mOm, Id("inbox"));
   if(inbox.size() == 0)
   {
       mReward.setMod(Id("mod.inbox"), "Clean inbox", 0.1);
   }
   else
   {
       mReward.removeMod(Id("mod.inbox"));
   }
}

static std::time_t to_time_t(const boost::gregorian::date& date )
{
	using namespace boost::posix_time;
	static ptime epoch(boost::gregorian::date(1970, 1, 1));
	time_duration::sec_type secs = (ptime(date,seconds(0)) - epoch).total_seconds();
	return std::time_t(secs);
}

void UserSS::advanceExpiredCalendarItems(const boost::gregorian::date& date)
{
   bool hasExpiredItems = false;
   auto curTime = to_time_t(date);
   auto curDayBegin = curTime - (curTime % 86400);
   for(auto o : mOm.getAll("calendar_item"))
   {
       if((o)["entityTypeChoice"].get<Type::Choice>() != "Event" && (o)["timestamp"].get<Type::Timestamp>().value < curTime)
       {
           o["timestamp"] = Time{curDayBegin + (o["timestamp"].get<Type::Timestamp>().value % 86400)};
           hasExpiredItems = true;
           mOm.modify(o);
       }
   }

   if(hasExpiredItems)
   {
       mReward.removeMod(Id("mod.calendar"));
   }
   else
   {
       mReward.setMod(Id("mod.calendar"), "Clean calendar", 0.1);
   }
}

void UserSS::onNewDay(const boost::gregorian::date& date)
{
   awardInbox();
   generateNewTOD();
   advanceExpiredCalendarItems(date);
}

void UserSS::onNewWeek()
{
    
}

std::vector<TypeDef> UserSS::getTypes()
{
   return {};
}

std::vector<CommandDef> UserSS::getCommandDefs()
{
    return {{"complete", std::bind(&UserSS::parseComplete, this, std::placeholders::_1)}};
}

}

