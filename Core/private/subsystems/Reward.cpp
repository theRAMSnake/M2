
#include "../ObjectManager.hpp"
#include "../ExceptionsUtil.hpp"
#include "../Logger.hpp"
#include "../rng.hpp"
#include "../types/Variable.hpp"
#include "Reward.hpp"

#include <boost/date_time/gregorian/greg_date.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/algorithm/string/replace.hpp>

namespace materia
{


RewardSS::RewardSS(ObjectManager& objMan)
: mOm(objMan)
{

}

std::vector<TypeDef> RewardSS::getTypes()
{
    std::vector<TypeDef> result;

    result.push_back({"reward_item", "reward_items", {
        {"name", Type::String},
        {"amount", Type::Int},
        }});

    result.push_back({"reward_contract", "contracts", {
        {"caption", Type::String},
        {"config_id", Type::String},
        {"reward", Type::Int},
        {"daysLeft", Type::Int},
        {"score", Type::Int},
        {"goal", Type::Int},
        {"reward_mod_id", Type::String}, //If mod is set - 1% of reward will be added to the mod
        }});

    result.push_back({"reward_modifier", "reward_modifiers", {
        {"desc", Type::String},
        {"value", Type::Double},
        {"validUntil", Type::Timestamp}
        }});

    return result;
}

void RewardSS::levelUpContract(const Id id)
{
    if(id != Id::Invalid)
    {
        auto cb = mOm.getOrCreate(Id("reward.cb"), "object");
        (cb)[id.getGuid()] = (cb)[id.getGuid()].get<Type::Int>() + 1;

        mOm.modify(cb);
    }
}

int getCurrentDayOfWeek()
{
    time_t rawtime;
    tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    return timeinfo->tm_wday;
}

static std::time_t to_time_t(const boost::gregorian::date& date )
{
	using namespace boost::posix_time;
	static ptime epoch(boost::gregorian::date(1970, 1, 1));
	time_duration::sec_type secs = (ptime(date,seconds(0)) - epoch).total_seconds();
	return std::time_t(secs);
}

void RewardSS::onNewDay(const boost::gregorian::date& date)
{
    const std::size_t MAX_CONTRACTS = 2;
    auto ctrs = mOm.getAll("reward_contract");

    int totalBonus = 0;
    for(auto& obj : ctrs)
    {
        if(obj["score"].get<Type::Int>() >= obj["goal"].get<Type::Int>())
        {
            auto modId = obj["reward_mod_id"].get<Type::String>();
            if(modId == "")
            {
                totalBonus += obj["reward"].get<Type::Int>();
            }
            else
            {
                //Mod reward
                auto mod = mOm.getOrCreate(Id(modId), "reward_modifier");
                mod["value"] = mod["value"].get<Type::Double>() + static_cast<double>(obj["reward"].get<Type::Int>()) / 100;
                mOm.modify(mod);
            }
            
            levelUpContract(obj["config_id"].get<Type::String>());
            mOm.destroy(obj.getId());
        }
        else
        {
            auto daysLeft = obj["daysLeft"].get<Type::Int>() - 1;
            if(daysLeft == 0)
            {
                mOm.destroy(obj.getId());
            }
            else
            {
                obj["daysLeft"] = daysLeft;
                mOm.modify(obj);
            }
        }
    }

    ctrs = mOm.getAll("reward_contract");
    if(ctrs.size() != MAX_CONTRACTS)
    {
        genContract();
    }

    if(totalBonus != 0)
    {
        addPoints(totalBonus);
    }

    types::Variable wb(mOm, Id("work.burden"));

    if(wb > 0)
    {
        setMod(Id("mod.workburden"), "Bad work", -0.2);
    }
    else
    {
        setMod(Id("mod.workburden"), "Good work", 0.1);
    }

    if(date.day_of_week() != boost::gregorian::Sunday && 
        date.day_of_week() != boost::gregorian::Saturday)
    {
        auto cfg = mOm.getOrCreate(Id("config.reward"), "object");
        auto wbpd = cfg["workburdenPerDay"].get<Type::Int>();

        wb.inc(wbpd);

        if(wb > 600)
        {
            wb = 600;
        }
    }

    types::Variable points(mOm, Id("reward.points"));
    if(points < 0)
    {
        setMod(Id("mod.punisher"), "Punisher", -0.15);
    }
    else
    {
        removeMod(Id("mod.punisher"));
    }

    auto curTime = to_time_t(date);
    for(auto o : mOm.getAll("reward_modifier"))
    {
       if(o["validUntil"].get<Type::Timestamp>().value < curTime)
       {
          removeMod(o.getId());
       }
    }
}

template<class F>
class FunctionToValueProviderAdapter : public IValueProvider
{
public:
    FunctionToValueProviderAdapter(F f)
    : mF(f)
    {

    }

    void populate(Object& obj) const override
    {
        mF(obj);
    }

private:
    F mF;
};

void RewardSS::genContract()
{
    auto cfg = mOm.getOrCreate(Id("config.reward"), "object");
    auto cb = mOm.getOrCreate(Id("reward.cb"), "object");

    try
    {
        auto options = cfg.getChild(Id("contracts")).getChildren();

        auto& randomItem = options[Rng::gen32() % options.size()];
        auto configId = randomItem.getId().getGuid();
        auto level = (cb)[configId].get<Type::Int>();

        auto valueProvider = FunctionToValueProviderAdapter([&randomItem, level, configId](auto& obj)
        {
            auto goal = randomItem["goal"].get<Type::Int>() + randomItem["goalGrowth"].get<Type::Double>() * level;
            auto time = randomItem["time"].get<Type::Int>() + randomItem["timeGrowth"].get<Type::Double>() * level;
            auto reward = randomItem["rewardBase"].get<Type::Int>() + randomItem["rewardPerLevel"].get<Type::Double>() * level;
            auto caption = randomItem["caption"].get<Type::String>();

            boost::replace_all(caption, "%", std::to_string(static_cast<std::int64_t>(std::round(goal))));

            obj["caption"] = caption;
            obj["config_id"] = configId;
            obj["reward"] = static_cast<std::int64_t>(std::round(reward));
            obj["daysLeft"] = static_cast<std::int64_t>(std::round(time));
            obj["score"] = 0;
            obj["reward_mod_id"] = randomItem["reward_mod_id"].get<Type::String>();
            obj["goal"] = static_cast<std::int64_t>(std::round(goal));
        });

        mOm.create(Id::generate(), "reward_contract", valueProvider);
    }
    catch(std::exception& ex)
    {
        LOG("Reward config is corrupted: " + std::string(ex.what()));
    }
}

void RewardSS::onNewWeek()
{
   
}

std::vector<CommandDef> RewardSS::getCommandDefs()
{
    return {{"reward", std::bind(&RewardSS::parseRewardCommand, this, std::placeholders::_1)}};
}

class RewardCommand : public Command
{
public:
   RewardCommand(const int pts, RewardSS& reward)
   : mPts(pts)
   , mReward(reward)
   {

   }

   ExecutionResult execute(ObjectManager& objManager) override
   {
      mReward.addPoints(mPts);
      return Success{};
   }

private:
    const int mPts;
    RewardSS& mReward;
};

Command* RewardSS::parseRewardCommand(const boost::property_tree::ptree& src)
{
   auto pts = getOrThrow<int>(src, "points", "Points is not specified");

   return new RewardCommand(pts, *this);
}

double RewardSS::calculateTotalModifier()
{
    double result = 0;

    for(auto &m: mOm.getAll("reward_modifier"))
    {
        result += m["value"].get<Type::Double>();
    }

    return result;
}

void RewardSS::addPoints(const int points)
{
    types::Variable debt(mOm, Id("reward.debt"));
    types::Variable pool(mOm, Id("reward.points"));

    if(debt > 0)
    {
        pool.dec(debt.asInt() * 100);
        debt = 0;
    }

    bool isPlus = points > 0;
    auto mod = calculateTotalModifier();
    auto pointsToAssign = static_cast<double>(points) * 100 * (isPlus ? 1.0 + mod : 1.0 - mod);

    pool.inc(pointsToAssign);

    while(pool > 2500)
    {
        pool.dec(2500);

        auto vp = FunctionToValueProviderAdapter([](auto& obj)
        {
            obj["name"] = "chest";
        });

        mOm.create({}, "reward_item", vp);
    }
}

void RewardSS::setMod(const Id& id, const std::string& desc, const double value)
{
    removeMod(id);

    auto valueProvider = FunctionToValueProviderAdapter([&desc, value](auto& obj)
    {
        obj["desc"] = desc;
        obj["value"] = value;
    });

    mOm.create(id, "reward_modifier", valueProvider);
}

void RewardSS::removeMod(const Id& id)
{
    mOm.destroy(id);
}
    
}
