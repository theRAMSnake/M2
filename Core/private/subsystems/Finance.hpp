#pragma once
#include "../ISubsystem.hpp"
#include "Reward.hpp"

namespace materia
{

class ObjectManager;
class FinanceSS : public ISubsystem
{
public:
    FinanceSS(ObjectManager& objMan, RewardSS& reward);
    void onNewDay() override;
    void onNewWeek() override;
    std::vector<TypeDef> getTypes() override;
    std::vector<CommandDef> getCommandDefs() override;

private:
    void performFinancialAnalisys();

    ObjectManager& mOm;
    RewardSS& mReward;
};

}