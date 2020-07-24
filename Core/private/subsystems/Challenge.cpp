#include "Challenge.hpp"
#include "../ObjectManager.hpp"

namespace materia
{

ChallengeSS::ChallengeSS(ObjectManager& objMan)
: mOm(objMan)
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
      if(obj["resetWeekly"])
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
    while(obj["points"] > obj["pointsNeeded"])
    {
        obj["points"] = static_cast<int>(obj["points"]) - static_cast<int>(obj["pointsNeeded"]);
        obj["level"] = static_cast<int>(obj["level"]) + 1;
        obj["pointsNeeded"] = static_cast<int>(obj["pointsNeeded"]) + static_cast<int>(obj["advance"]);

        mOm.LEGACY_getReward().addPoints(static_cast<int>(obj["rewardPerLevel"]));
    }
}

}

