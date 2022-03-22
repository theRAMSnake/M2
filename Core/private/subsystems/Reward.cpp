
#include "../ObjectManager.hpp"
#include "../ExceptionsUtil.hpp"
#include "../Logger.hpp"
#include "../rng.hpp"
#include "../types/Variable.hpp"
#include "../types/SimpleList.hpp"
#include "Reward.hpp"

#include <boost/date_time/gregorian/greg_date.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/cxx11/any_of.hpp>

#include <algorithm>

namespace materia
{

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

const std::vector<std::string> COIN_COLORS = {"Red", "Green", "Purple", "Yellow", "Blue"};
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
        {"validUntil", Type::Timestamp},
        {"expirable", Type::Bool}
        }});

    result.push_back({"reward_generator", "reward_generators", {
        {"desc", Type::String},
        {"value", Type::Int},
        {"color", Type::String},
        {"type", Type::Choice, {"Random", "Specific"}},
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

void consumeChest(ObjectManager& om)
{
    auto items = om.getAll("reward_item");
    auto chestPos = std::find_if(items.begin(), items.end(), [](auto& c){
            Field f = c["name"];
            std::string src = f.get<Type::String>();
            return src == "chest";});

    if(chestPos == items.end())
    {
        throw std::logic_error("No chest available");
    }

    om.destroy(chestPos->getId());
}

Object weightedRandom(const std::vector<Object>& srcs)
{
    for(int i = 0; i < 5000; ++i)
    {
        auto& randomItem = srcs[Rng::gen32() % srcs.size()];
        auto weight = randomItem["weight"].get<Type::Double>();

        LOG("Trying to get " + randomItem["name"].get<Type::String>());
        if(Rng::genProbability(weight))
        {
            LOG("YES!");
            return randomItem;
        }
        LOG("NO");
    }
    
    throw std::logic_error("Unable to generate weightedRandom after 5000 attempts");
}

Object selectChestType(ObjectManager& om)
{
    auto cfg = om.getOrCreate(Id("config.reward"), "object");
    auto options = cfg.getChild(Id("chests")).getChildren();
    return weightedRandom(options);
}

Object pickItem(const std::vector<Object>& cfg, const std::string& category)
{
    std::vector<Object> options;
    for(const auto& item : cfg)
    {
        if(item["category"].get<Type::String>() == category)
        {
            options.push_back(item);
        } 
    }
    if(options.empty())
    {
        throw std::logic_error("Broken reward config, cannot generate item of category: " + category);
    }
    return options[Rng::gen32() % options.size()];
}

Object pickItem(const std::vector<Object>& cfg, const std::vector<std::string>& categories, const int value)
{
    std::vector<Object> options;
    for(const auto& item : cfg)
    {
        auto category = item["category"].get<Type::String>();
        if(std::find(categories.begin(), categories.end(), category) != categories.end() && item["value"].get<Type::Int>() <= value)
        {
            options.push_back(item);
        } 
    }
    if(options.empty())
    {
        throw std::logic_error("Broken reward config, cannot generate item of categories and value: " + std::to_string(value));
    }
    return options[Rng::gen32() % options.size()];
}

void createMod(ObjectManager& om, const std::string& name, const double value, const int duration)
{
    auto valueProvider = FunctionToValueProviderAdapter([&name, value, duration](auto& obj)
    {
    	auto validUntil = std::chrono::system_clock::now() + std::chrono::days(duration);
        obj["desc"] = name;
        obj["value"] = value;
        obj["validUntil"] = Time{std::chrono::system_clock::to_time_t(validUntil)};
        obj["expirable"] = true;
    });

    om.create({}, "reward_modifier", valueProvider);
}

bool isStackable(const std::string& name)
{
    return name.find("Token") != std::string::npos;
}

void applyItem(ObjectManager& om, const Object& item)
{
    auto name = item["name"].get<Type::String>();
    if(item.contains("behavior"))
    {
        auto behavior = item.getChild("behavior");
        auto type = behavior["type"].get<Type::String>();
        if(type == "add_mod")
        {
            createMod(om, behavior["name"].get<Type::String>(), behavior["value"].get<Type::Double>(), behavior["duration"].get<Type::Int>());
        }
        else if(type == "fetch_from_list")
        {
            types::SimpleList lst(om, Id(behavior["source"].get<Type::String>()));
            auto vp = FunctionToValueProviderAdapter([&lst](auto& obj)
            {
                obj["name"] = lst.at(Rng::gen32() % lst.size());
            });

            om.create({}, "reward_item", vp);
        }
        else
        {
            throw std::logic_error("Unknown item behavior type: " + type);
        }
    }
    else
    {
        if(isStackable(name))
        {
            auto items = om.getAll("reward_item");
            bool found = false;
            for(auto& x : items)
            {
                if(x["name"].get<Type::String>() == name) 
                {
                    x["amount"] = x["amount"].get<Type::Int>() + (item.contains("amount") ? item["amount"].get<Type::Int>() : 1);
                    om.modify(x);
                    found = true;
                }
            }

            if(!found)
            {
                auto vp = FunctionToValueProviderAdapter([name](auto& obj)
                {
                    obj["name"] = name;
                    obj["amount"] = 1;
                });

                om.create({}, "reward_item", vp);
            }
        }
        else
        {
            auto vp = FunctionToValueProviderAdapter([name](auto& obj)
            {
                obj["name"] = name;
            });

            om.create({}, "reward_item", vp);
        }
    }
}

Object applyChest(ObjectManager& om, const Object& chestType)
{
    Object report({"object"}, Id("report"));
    std::vector<std::string> gainedItems;
    report["chestType"] = chestType["name"].get<Type::String>();

    auto cfg = om.getOrCreate(Id("config.reward"), "object");
    auto itemsCfg = cfg.getChild(Id("items")).getChildren();
    auto chestValuesLeft = cfg["chestValues"].get<Type::Int>();
    
    if(chestType.contains("premium"))
    {
        auto premium = chestType["premium"].get<Type::String>();
        auto newItemCfg = pickItem(itemsCfg, premium);
        gainedItems.push_back(newItemCfg["name"].get<Type::String>());
        chestValuesLeft -= newItemCfg["value"].get<Type::Int>();
        applyItem(om, newItemCfg);
    }

    if(chestType.contains("fillwith"))
    {
        std::vector<std::string> fillwith;
        boost::split(fillwith, chestType["fillwith"].get<Type::String>(), boost::is_any_of(";"));
        while(chestValuesLeft > 0)
        {
            auto newItemCfg = pickItem(itemsCfg, fillwith, chestValuesLeft);
            gainedItems.push_back(newItemCfg["name"].get<Type::String>());
            chestValuesLeft -= newItemCfg["value"].get<Type::Int>();
            applyItem(om, newItemCfg);
        }
    }

    report["gainedItems"] = gainedItems;

    return report;
}

Object RewardSS::useChest()
{
    consumeChest(mOm);
    auto chestType = selectChestType(mOm);
    return applyChest(mOm, chestType);
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
        //Disabled until contracts support new system
        //addPoints(totalBonus);
    }

    types::Variable wb(mOm, Id("work.burden"));

    if(wb > 0)
    {
        setModAndGenerator(Id("mod.workburden"), "Bad work", -0.2, -2, "Purple");
    }
    else
    {
        setModAndGenerator(Id("mod.workburden"), "Good work", 0.1, 1, "Purple");
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

    auto curTime = to_time_t(date);
    for(auto o : mOm.getAll("reward_modifier"))
    {
       if(o["expirable"].get<Type::Bool>() && o["validUntil"].get<Type::Timestamp>().value < curTime)
       {
           removeModAndGenerator(o.getId());
       }
    }

    auto coins = mOm.getOrCreate(Id("reward.coins"), "object");
    for(auto o : mOm.getAll("reward_generator"))
    {
        std::string color;
        if(o["type"].get<Type::Choice>() == "Random")
        {
            color = COIN_COLORS[Rng::genChoise(COIN_COLORS.size())];
        }
        else if(o["type"].get<Type::Choice>() == "Specific")
        {
            color = o["color"].get<Type::String>();
        }

        if(coins.contains(color))
        {
            coins[color] = std::max(0, static_cast<int>(coins[color].get<Type::Int>() + o["value"].get<Type::Int>()));
        }
        else
        {
            //Error
        }
    }
    mOm.modify(coins);
}


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
   RewardCommand(const int coins, const std::string& color, RewardSS& reward)
   : mCoins(coins)
   , mColor(color)
   , mReward(reward)
   {

   }

   ExecutionResult execute(ObjectManager& objManager) override
   {
      mReward.addCoins(mCoins, mColor);
      return Success{};
   }

private:
    const int mCoins;
    const std::string mColor;
    RewardSS& mReward;
};

class UseChestCommand : public Command
{
public:
   UseChestCommand(RewardSS& reward)
   : mReward(reward)
   {

   }

   ExecutionResult execute(ObjectManager& objManager) override
   {
      try 
      {
          return std::vector<Object>{mReward.useChest()};
      }
      catch(std::exception& e)
      {
          throw std::logic_error{std::string("Unable to use a chest: ") + e.what()};
      }
   }

private:
    RewardSS& mReward;
};

Command* RewardSS::parseUseChestCommand(const boost::property_tree::ptree& src)
{
   return new UseChestCommand(*this);
}

Command* RewardSS::parseRewardCommand(const boost::property_tree::ptree& src)
{
   auto coins = getOrThrow<int>(src, "coins", "Coins is not specified");
   auto color = getOrThrow<std::string>(src, "color", "Color is not specified");

   return new RewardCommand(coins, color, *this);
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

void RewardSS::addCoins(const int coins, const std::string& color)
{
    std::string colorToUse = color;
    if(color == "Random")
    {
        colorToUse = COIN_COLORS[Rng::genChoise(COIN_COLORS.size())];
    }

    if(std::find(COIN_COLORS.begin(), COIN_COLORS.end(), colorToUse) == COIN_COLORS.end())
    {
        throw std::runtime_error("Unsupported coin color in addCoins: " + colorToUse);
    }

    auto coinsStash = mOm.getOrCreate(Id("reward.coins"), "object");
    coinsStash[color] = static_cast<int>(coinsStash[color].get<Type::Int>() + coins);
    mOm.modify(coinsStash);
}

void RewardSS::setModAndGenerator(const Id& id, const std::string& desc, const double value, const int valueInt, const std::optional<std::string>& color)
{
    removeModAndGenerator(id);

    {
        auto valueProvider = FunctionToValueProviderAdapter([&desc, value](auto& obj)
        {
            obj["desc"] = desc;
            obj["value"] = value;
        });

        mOm.create(id, "reward_modifier", valueProvider);
    }
    {
        auto valueProvider = FunctionToValueProviderAdapter([&desc, &valueInt, &color](auto& obj)
        {
            obj["desc"] = desc;
            obj["value"] = valueInt;
            if(color)
            {
                obj["type"] = "Specific";
                obj["color"] = *color;
            }
            else
            {
                obj["type"] = "Random";
            }
        });

        mOm.create(Id("Generator" + id.getGuid()), "reward_generator", valueProvider);
    }
}

void RewardSS::removeModAndGenerator(const Id& id)
{
    mOm.destroy(id);
    mOm.destroy(Id("Generator" + id.getGuid()));
}

}
