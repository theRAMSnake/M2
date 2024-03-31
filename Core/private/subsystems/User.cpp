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
#include <iostream>

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

void complete(const Id id, ObjectManager& om, RewardSS& reward)
{
    auto object = om.get(id);

    if(object.getType().name == "calendar_item")
    {
        auto eType = object["entityTypeChoice"].get<Type::Choice>();
        auto recPeriod = object["recurrency"].get<Type::Period>();
        if(eType == "Task")
        {
            auto cons = om.getConnections().get(id);
            for(auto& c : cons)
            {
                if(c.a == id && c.type == ConnectionType::Reference)
                {
                    auto ref = om.get(c.b);
                    if(ref.getType().name == "core_value")
                    {
                        reward.addCoins(1, ref["color"].get<Type::String>());
                    }
                }
            }
        }
        else if(eType == "StrategyNodeReference")
        {
            auto cons = om.getConnections().get(id);
            auto pos = std::find_if(cons.begin(), cons.end(), [&](auto x){
                return x.a == id && x.type == ConnectionType::Reference;
                });

            if(pos != cons.end())
            {
                Object obj = om.get(pos->b);
                auto tp = obj["typeChoice"].get<Type::Choice>();
                if(tp == "Goal" || tp == "Task")
                {
                    complete(pos->b, om, reward);
                }
                //If counter -> ++
                else if(tp == "Counter")
                {
                    obj["value"] = obj["value"].get<Type::Int>() + 1;
                    om.modify(obj);

                    if(obj["value"].get<Type::Int>() >= obj["target"].get<Type::Int>())
                    {
                        complete(pos->b, om, reward);
                    }
                }
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
    else if(object.getType().name == "strategy_node")
    {
        if(object["isAchieved"].get<Type::Bool>())
        {
            throw std::runtime_error("Strategy node already completed");
        }
        auto eType = object["typeChoice"].get<Type::Choice>();
        if(eType == "Task" || eType == "Goal")
        {
            //pass
        }
        else if(eType == "Counter")
        {
            if(object["value"].get<Type::Int>() < object["target"].get<Type::Int>())
            {
                throw std::runtime_error("Cannot complete counter with value < target");
            }
        }
        else
        {
            throw std::runtime_error(fmt::format("Strategy node of type {} is not completable", eType));
        }

        auto cons = om.getConnections().get(object.getId());
        for(auto& c : cons)
        {
            if(c.a == object.getId() && c.type == ConnectionType::Reference)
            {
                auto ref = om.get(c.b);
                if(ref.getType().name == "core_value")
                {
                    reward.addCoins(object["reward"].get<Type::Int>(), ref["color"].get<Type::String>());
                    break;
                }
            }
        }
        object["isAchieved"] = true;
        om.modify(object);
    }
    else
    {
        throw std::runtime_error(fmt::format("Type {} is not completable", object.getType().name));
    }

}

class CompleteCommand : public Command
{
public:
   CompleteCommand(const Id& id, RewardSS& reward)
   : mId(id)
   , mReward(reward)
   {

   }

   ExecutionResult execute(ObjectManager& objManager) override
   {
      complete(mId, objManager, mReward);
      return Success{};
   }

private:
    const Id mId;
    RewardSS& mReward;
};

Command* UserSS::parseComplete(const boost::property_tree::ptree& src)
{
   auto id = getOrThrow<Id>(src, "id", "Id is not specified");

   return new CompleteCommand(id, mReward);
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
       mReward.setGenerator(Id("mod.inbox"), "Clean inbox", 1, {});
   }
   else
   {
       mReward.removeGenerator(Id("mod.inbox"));
   }

   types::Variable hl(mOm, Id("health.level"));
   mReward.setGenerator(Id("mod.health"), "Health bonus", hl.asInt() / 10, "Green");
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
       if((o)["entityTypeChoice"].get<Type::Choice>() != "Event" && (o)["timestamp"].get<Type::Timestamp>().value < curTime)
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

