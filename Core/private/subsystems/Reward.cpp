
#include "../ObjectManager.hpp"
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


}