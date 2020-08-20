#pragma once
#include "../ISubsystem.hpp"
#include "Reward.hpp"
#include "Common.hpp"

namespace materia
{

class ObjectManager;
class FinanceSS : public ISubsystem
{
public:
    FinanceSS(ObjectManager& objMan, RewardSS& reward, CommonSS& common);
    void onNewDay() override;
    void onNewWeek() override;
    std::vector<TypeDef> getTypes() override;
    std::vector<CommandDef> getCommandDefs() override;

private:
    void performFinancialAnalisys();

    ObjectManager& mOm;
    RewardSS& mReward;
    CommonSS& mCommon;
};

}