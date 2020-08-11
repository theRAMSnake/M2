#include "Challenge.hpp"
#include "../ObjectManager.hpp"

namespace materia
{

ChallengeSS::ChallengeSS(ObjectManager& objMan, RewardSS& reward)
: mOm(objMan)
, mReward(reward)
{

}

void ChallengeSS::onNewDay()
{

}

void ChallengeSS::onNewWeek()
{
   //reset challenge items
   auto objList = mOm.getAll("challenge_item");
   for(auto o : objList)
   {
      auto& obj = *o;
      if(obj["resetWeekly"].get<Type::Bool>())
      {
         obj["points"] = 0;
         mOm.modify(obj);
      }
   }
}

std::vector<TypeDef> ChallengeSS::getTypes()
{
    std::vector<TypeDef> result;

    result.push_back({"challenge_item", "challenge_new", {
        {"brief", Type::String},
        {"points", Type::Int},
        {"pointsNeeded", Type::Int},
        {"level", Type::Int},
        {"advance", Type::Int},
        {"rewardPerLevel", Type::Int},
        {"resetWeekly", Type::Bool},
        }});
    result.back().handlers.onChanged = std::bind(&ChallengeSS::handleChItemChange, this, std::placeholders::_1);

    return result;
}

void ChallengeSS::handleChItemChange(Object& obj)
{
    while(obj["points"].get<Type::Int>() > obj["pointsNeeded"].get<Type::Int>())
    {
        obj["points"] = obj["points"].get<Type::Int>() - obj["pointsNeeded"].get<Type::Int>();
        obj["level"] = obj["level"].get<Type::Int>() + 1;
        obj["pointsNeeded"] = obj["pointsNeeded"].get<Type::Int>() + obj["advance"].get<Type::Int>();

        mReward.addPoints(obj["rewardPerLevel"].get<Type::Int>());
    }
}

std::vector<CommandDef> ChallengeSS::getCommandDefs()
{
    return {};
}

}

