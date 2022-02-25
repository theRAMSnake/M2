#include "User.hpp"
#include "../ObjectManager.hpp"
#include "Strategy.hpp"
#include "../types/SimpleList.hpp"
#include "../types/Variable.hpp"
#include "../ExceptionsUtil.hpp"
#include "../rng.hpp"
#include <boost/date_time/gregorian/greg_date.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/algorithm/string.hpp>

namespace materia
{

Time advance(const Time src, const Period& period)
{
    Time result = src;

    auto bdt = boost::posix_time::from_time_t(src.value);
    
    std::cout << std::to_string(period);

    bdt += period.years;
    bdt += period.months;
    bdt += period.days;

    result.value = boost::posix_time::to_time_t(bdt);

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
    auto recPeriod = object["recurrency"].get<Type::Period>();
    if(eType == "Task")
    {
        reward.addPoints(1);
    }
    else if(eType == "StrategyNodeReference")
    {
        auto cons = om.getConnections().get(id);
        auto pos = std::find_if(cons.begin(), cons.end(), [&](auto x){
            return x.a == id && x.type == ConnectionType::Reference;
            });

        if(pos != cons.end())
        {
            strategy.onCalendarReferenceCompleted(pos->b);
        }
        else
        {
            throw std::runtime_error("No reference found for strategy node reference: " + id.getGuid());
        }
    }

    if(!(recPeriod == Period::Empty()))
    {
        object["timestamp"] = advance(object["timestamp"].get<Type::Timestamp>(), recPeriod);
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
       mReward.setMod(Id("mod.inbox"), "Clean inbox", 0.05);
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
   updatePortfolio();
}

void UserSS::onNewWeek()
{
    
}

void UserSS::updatePortfolio()
{
    try
    {
        auto snp = mOm.get(Id("data.snp"));
        auto cfg = mOm.get(Id("config.invest"));

        auto totalMoney = cfg["snpgoal"].get<Type::Int>();

        double totalWeight = 0.0;
        for(auto c : snp.getChildren())
        {
            totalWeight += c["weight"].get<Type::Double>();
        }

        auto moneyPerWeight = totalMoney / totalWeight;

        mOm.destroy(Id("portfolio_goal"));

        auto result = mOm.getOrCreate(Id("portfolio_goal"), "object");

        for(auto c : snp.getChildren())
        {
            Object item(c);
            item["amount"] = (item["weight"].get<Type::Double>() * moneyPerWeight) / item["price"].get<Type::Double>();

            auto key = item["ticker"].get<Type::String>();
            boost::algorithm::replace_all(key, ".", "_");

            result.setChild(key, item);
        }

        mOm.modify(result);
    }
    catch(...)
    {

    }
}

std::vector<TypeDef> UserSS::getTypes()
{
    std::vector<TypeDef> result;

    result.push_back({"core_value", "core_values", {
        {"name", Type::String},
        {"color", Type::String}
        }});

    return result;
}

std::vector<CommandDef> UserSS::getCommandDefs()
{
    return {{"complete", std::bind(&UserSS::parseComplete, this, std::placeholders::_1)}};
}

}

