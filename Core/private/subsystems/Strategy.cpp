#include "Strategy.hpp"
#include "../ObjectManager.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>

namespace materia
{

StrategySS::StrategySS(ObjectManager& objMan, RewardSS& reward)
: mOm(objMan)
, mReward(reward)
{

}

static std::time_t to_time_t(const boost::gregorian::date& date )
{
	using namespace boost::posix_time;
	static ptime epoch(boost::gregorian::date(1970, 1, 1));
	time_duration::sec_type secs = (ptime(date,seconds(0)) - epoch).total_seconds();
	return std::time_t(secs);
}

void StrategySS::onNewDay(const boost::gregorian::date& date)
{
   for(auto o : mOm.getAll("strategy_node"))
   {
       if((o)["typeChoice"].get<Type::Choice>() == "Wait" && (o)["date"].get<Type::Timestamp>().value < to_time_t(date))
       {
           (o)["isAchieved"] = true;
           mOm.modify(o);
       }
   }
}

void StrategySS::onNewWeek()
{
}

std::vector<std::string> allowedTypes = {"Goal", "Task", "Counter", "Watch", "Wait"};

std::vector<TypeDef> StrategySS::getTypes()
{
    std::vector<TypeDef> result;

    result.push_back({"strategy_node", "strategy_nodes", {
        {"title", Type::String},
        {"details", Type::String},
        {"typeChoice", Type::Choice, allowedTypes},
        {"isAchieved", Type::Bool},
        {"date", Type::Timestamp},
        {"value", Type::Int},
        {"target", Type::Int},
        {"reward", Type::Int}
        }, {},
        {{"core_value", ConnectionType::Reference, "Core Value"}}
        });

    return result;
}

std::vector<CommandDef> StrategySS::getCommandDefs()
{
    return {};
}
}

