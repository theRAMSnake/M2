
#include "../ObjectManager.hpp"
#include "../ExceptionsUtil.hpp"
#include "Reward.hpp"

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

    return result;
}

void RewardSS::onNewDay()
{

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

void RewardSS::addPoints(const int points)
{
    auto pools = mOm.getAll("reward_pool");
    unsigned int attemptCounter = 0;
    int pointsLeft = std::abs(points);
    bool isPlus = points > 0;

    while(!pools.empty() && pointsLeft > 0)
    {
        auto& randomItem = *pools[rand() % pools.size()];
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
       mOm.modify(*p);
    }
}

}