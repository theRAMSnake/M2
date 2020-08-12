#pragma once
#include "../ISubsystem.hpp"
#include "Reward.hpp"

namespace materia
{

class ObjectManager;
class ChallengeSS : public ISubsystem
{
public:
    ChallengeSS(ObjectManager& objMan, RewardSS& reward);
    void onNewDay() override;
    void onNewWeek() override;
    std::vector<TypeDef> getTypes() override;
    std::vector<CommandDef> getCommandDefs() override;

private:
    void handleChItemChange(Object& obj);

    ObjectManager& mOm;
    RewardSS& mReward;
};

}