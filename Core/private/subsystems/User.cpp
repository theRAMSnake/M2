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

Time advance(const Time src, const int recType)
{
    Time result = src;

    switch (recType)
    {
    case 1:
        result.value += 604800;
        break;

    case 2:
        result.value = boost::posix_time::to_time_t(boost::posix_time::from_time_t(result.value) + boost::gregorian::months(1));
        break;

    case 3:
        result.value = boost::posix_time::to_time_t(boost::posix_time::from_time_t(result.value) + boost::gregorian::months(3));
        break;

    case 4:
        result.value = boost::posix_time::to_time_t(boost::posix_time::from_time_t(result.value) + boost::gregorian::months(12));
        break;

    case 5:
        result.value += 172800;
        break;
    
    default:
        break;
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

    auto eType = object["entityType"].get<Type::Option>();
    auto recType = object["reccurencyType"].get<Type::Option>();
    if(eType == 1/*task*/)
    {
        reward.addPoints(1);
    }
    else if(eType == 2/*strategy ref*/)
    {
        strategy.onCalendarReferenceCompleted(object["nodeReference"].toId());
    }

    if(recType != 0/*none*/)
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
   if(inbox.size() == 0 && Rng::genProbability(0.1))
   {
      mReward.addPoints(1);
      inbox.add("Extra point awarded for empty inbox.");
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
   auto curTime = to_time_t(date);
   auto curDayBegin = curTime - (curTime % 86400);
   for(auto o : mOm.getAll("calendar_item"))
   {
       if((o)["entityType"].get<Type::Option>() != 0/*Event*/ && (o)["timestamp"].get<Type::Timestamp>().value < curTime)
       {
           o["timestamp"] = Time{curDayBegin + (o["timestamp"].get<Type::Timestamp>().value % 86400)};
           
           mOm.modify(o);
       }
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

