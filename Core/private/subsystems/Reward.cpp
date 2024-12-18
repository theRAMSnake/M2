
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

Object weightedRandom(const std::vector<Object>& srcs)
{
    for(int i = 0; i < 5000; ++i)
    {
        auto& randomItem = srcs[Rng::gen32() % srcs.size()];
        auto weight = randomItem["weight"].get<Type::Double>();

        if(Rng::genProbability(weight))
        {
            return randomItem;
        }
    }

    throw std::logic_error("Unable to generate weightedRandom after 5000 attempts");
}

void populateCoinCost(Object& o, const std::string& costBase)
{
    std::vector<std::string> amount;
    boost::split(amount, costBase, boost::is_any_of(" "));

    for(auto& c : amount)
    {
        auto color = COIN_COLORS[Rng::genChoise(COIN_COLORS.size())];

        o["cost" + color] = static_cast<int>(o["cost" + color].get<Type::Int>() + std::stoi(c));
    }
}

void RewardSS::spawnShopItems()
{
    auto allSpawners = mOm.getAll("reward_shop_spawner");
    if(allSpawners.empty())
    {
        return;
    }

    auto cfg = mOm.getOrCreate(Id("config.reward"), "object");
    const std::size_t MAX_ITEMS = cfg["shopSize"].get<Type::Int>();
    auto currentCount = mOm.getAll("reward_shop_item").size();

    for(std::size_t i = currentCount; i < MAX_ITEMS; ++i)
    {
        auto item = weightedRandom(allSpawners);
        auto vp = FunctionToValueProviderAdapter([&item, this](auto& obj)
        {
            if(item["type"].get<Type::Choice>() == "Assign")
            {
                obj["name"] = item["name"];
                obj["amount"] = item["amount"];
            }
            else
            {
                types::SimpleList lst(mOm, Id(item["listId"].get<Type::String>()));
                obj["name"] = lst.at(Rng::gen32() % lst.size());
                obj["amount"] = 1;
            }
            populateCoinCost(obj, item["cost_base"].get<Type::String>());
        });

        mOm.create({}, "reward_shop_item", vp);
    }
}

std::vector<TypeDef> RewardSS::getTypes()
{
    std::vector<TypeDef> result;

    result.push_back({"reward_item", "reward_items", {
        {"name", Type::String},
        {"amount", Type::Int},
        }});

    result.push_back({"reward_shop_item", "reward_shop_items", {
        {"name", Type::String},
        {"amount", Type::Int},
        {"costRed", Type::Int},
        {"costBlue", Type::Int},
        {"costYellow", Type::Int},
        {"costPurple", Type::Int},
        {"costGreen", Type::Int},
        }});

    result.push_back({"reward_shop_spawner", "reward_shop_spawners", {
        {"name", Type::String},
        {"amount", Type::Int},
        {"weight", Type::Double},
        {"cost_base", Type::String},
        {"type", Type::Choice, {"Assign", "FetchFromList"}},
        {"listId", Type::String}
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

bool isStackable(const std::string& name)
{
    return name.find("Token") != std::string::npos;
}

void RewardSS::onNewDay(const boost::gregorian::date& date)
{
    types::Variable wb(mOm, Id("work.burden"));

    if(wb > 0)
    {
        setGenerator(Id("mod.workburden"), "Bad work", -2, "Purple");
    }
    else
    {
        setGenerator(Id("mod.workburden"), "Good work", 1, "Purple");
    }

    if(date.day_of_week() != boost::gregorian::Sunday &&
        date.day_of_week() != boost::gregorian::Saturday)
    {
        auto cfg = mOm.getOrCreate(Id("config.reward"), "object");
        auto wbpd = cfg["workburdenPerDay"].get<Type::Int>();

        wb.inc(wbpd);
    }

    auto coins = mOm.getOrCreate(Id("reward.coins"), "object");
    for(auto o : mOm.getAll("reward_generator"))
    {
        //std::cout << "Generating for: " << o["desc"].get<Type::String>() << "\n";
        //std::cout << " value :" << o["value"].get<Type::Int>() << "\n";
        //std::cout << " color :" << o["color"].get<Type::String>() << "\n";
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

void RewardSS::onNewWeek()
{

}

std::vector<CommandDef> RewardSS::getCommandDefs()
{
    return {{"reward", std::bind(&RewardSS::parseRewardCommand, this, std::placeholders::_1)},
        {"buyRewardItem", std::bind(&RewardSS::parseBuyCommand, this, std::placeholders::_1)}
    };
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

Command* RewardSS::parseRewardCommand(const boost::property_tree::ptree& src)
{
   auto coins = getOrThrow<int>(src, "coins", "Coins is not specified");
   auto color = getOrThrow<std::string>(src, "color", "Color is not specified");

   return new RewardCommand(coins, color, *this);
}

class BuyCommand : public Command
{
public:
   BuyCommand(const Id id, RewardSS& reward)
   : mId(id)
   , mReward(reward)
   {

   }

   ExecutionResult execute(ObjectManager& objManager) override
   {
      mReward.buyRewardItem(mId);
      return Success{};
   }

private:
    const Id mId;
    RewardSS& mReward;
};

bool checkCost(const Object& target, const Object& coins)
{
    return target["costRed"].get<Type::Int>() <= coins["Red"].get<Type::Int>()
        && target["costBlue"].get<Type::Int>() <= coins["Blue"].get<Type::Int>()
        && target["costPurple"].get<Type::Int>() <= coins["Purple"].get<Type::Int>()
        && target["costGreen"].get<Type::Int>() <= coins["Green"].get<Type::Int>()
        && target["costYellow"].get<Type::Int>() <= coins["Yellow"].get<Type::Int>();
}

void subtractCost(const Object& target, Object& coins)
{
    coins["Red"] = coins["Red"].get<Type::Int>() - target["costRed"].get<Type::Int>();
    coins["Blue"] = coins["Blue"].get<Type::Int>() - target["costBlue"].get<Type::Int>();
    coins["Purple"] = coins["Purple"].get<Type::Int>() - target["costPurple"].get<Type::Int>();
    coins["Green"] = coins["Green"].get<Type::Int>() - target["costGreen"].get<Type::Int>();
    coins["Yellow"] = coins["Yellow"].get<Type::Int>() - target["costYellow"].get<Type::Int>();
}

void RewardSS::buyRewardItem(const Id id)
{
    auto target = mOm.get(id);
    auto coins = mOm.get(Id("reward.coins"));

    if(!checkCost(target, coins))
    {
        throw std::runtime_error("Not enough coins");
    }

    const auto name = target["name"].get<Type::String>();
    if(isStackable(name))
    {
        auto items = mOm.getAll("reward_item");
        bool found = false;
        for(auto& x : items)
        {
            if(x["name"].get<Type::String>() == name)
            {
                x["amount"] = x["amount"].get<Type::Int>() + target["amount"].get<Type::Int>();
                mOm.modify(x);
                found = true;
            }
        }

        if(!found)
        {
            auto vp = FunctionToValueProviderAdapter([name, target](auto& obj)
            {
                obj["name"] = name;
                obj["amount"] = target["amount"].get<Type::Int>();
            });

            mOm.create({}, "reward_item", vp);
        }
    }
    else
    {
        auto vp = FunctionToValueProviderAdapter([name, target](auto& obj)
        {
            obj["name"] = name;
            obj["amount"] = target["amount"].get<Type::Int>();
        });

        mOm.create({}, "reward_item", vp);
    }

    subtractCost(target, coins);
    mOm.modify(coins);
    mOm.destroy(target.getId());

    spawnShopItems();
}

Command* RewardSS::parseBuyCommand(const boost::property_tree::ptree& src)
{
   auto id = getOrThrow<std::string>(src, "targetId", "Target id is not specified");

   return new BuyCommand(id, *this);
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
    coinsStash[colorToUse] = std::max(0, static_cast<int>(coinsStash[colorToUse].get<Type::Int>() + coins));

    types::Variable discipline_level(mOm, Id("discipline.level"));
    auto val = discipline_level.asInt();
    auto chance = 1 - 100.0 / static_cast<double>(val + 100);
    for(int i = 0; i < coins; ++i) {
        if(val != 0 && Rng::genProbability(chance)) {
            coinsStash["Gold"] = coinsStash["Gold"].get<Type::Int>() + 1;
        }
    }
    mOm.modify(coinsStash);
}

void RewardSS::setGenerator(const Id& id, const std::string& desc, const int valueInt, const std::optional<std::string>& color)
{
    removeGenerator(id);

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

void RewardSS::removeGenerator(const Id& id)
{
    mOm.destroy(Id("Generator" + id.getGuid()));
}

}
