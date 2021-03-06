
#include "../ObjectManager.hpp"
#include "../ExceptionsUtil.hpp"
#include "../Logger.hpp"
#include "../rng.hpp"
#include "../types/Variable.hpp"
#include "Reward.hpp"

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

    result.push_back({"reward_pool", "reward", {
        {"name", Type::String},
        {"amount", Type::Int},
        {"amountMax", Type::Int}
        }});

    result.push_back({"reward_contract", "contracts", {
        {"caption", Type::String},
        {"config_id", Type::String},
        {"reward", Type::Int},
        {"daysLeft", Type::Int},
        {"score", Type::Int},
        {"goal", Type::Int}
        }});

    result.push_back({"reward_modifier", "reward_modifiers", {
        {"desc", Type::String},
        {"value", Type::Double}
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

void RewardSS::onNewDay(const boost::gregorian::date& date)
{
    const std::size_t MAX_CONTRACTS = 3;
    auto ctrs = mOm.getAll("reward_contract");

    int totalBonus = 0;
    for(auto& obj : ctrs)
    {
        if(obj["score"].get<Type::Int>() >= obj["goal"].get<Type::Int>())
        {
            totalBonus += obj["reward"].get<Type::Int>();
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

    types::Variable debt(mOm, Id("reward.debt"));
    if(debt > 0)
    {
        setMod(Id("mod.punisher"), "Punisher", -0.15);
    }
    else
    {
        removeMod(Id("mod.punisher"));
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
    bool isPlus = points > 0;
    auto mod = calculateTotalModifier();
    auto pointsModified = static_cast<double>(points) + static_cast<double>(points) * (isPlus ? mod : -mod) + mLeftOver;
    int pointsLeft = std::abs(static_cast<int>(pointsModified));
    mLeftOver = pointsModified - static_cast<int>(pointsModified);

    auto pools = mOm.getAll("reward_pool");
    unsigned int attemptCounter = 0;

    types::Variable debt(mOm, Id("reward.debt"));

    while(!pools.empty() && pointsLeft > 0)
    {
        if(debt > 0)
        {
            debt.inc(-1);
            pointsLeft--;
            continue;
        }

        auto& randomItem = pools[rand() % pools.size()];
        auto amount = randomItem["amount"].get<Type::Int>();
        if(isPlus && amount < randomItem["amountMax"].get<Type::Int>())
        {
            randomItem["amount"] = amount + 1;
            
            pointsLeft--;
        }
        else if(!isPlus && amount > 0)
        {
            randomItem["amount"] = amount - 1;
            pointsLeft--;
        }
        else
        {
            attemptCounter++;
            if(attemptCounter == 100)
            {
                break;
            }
        }
    }

    for(auto& p : pools)
    {
       mOm.modify(p);
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