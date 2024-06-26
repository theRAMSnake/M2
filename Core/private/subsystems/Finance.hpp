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
    void onNewDay(const boost::gregorian::date& date) override;
    void onNewWeek() override;
    std::vector<TypeDef> getTypes() override;
    std::vector<CommandDef> getCommandDefs() override;

private:
    void performFinancialAnalisys(const boost::gregorian::date& date);

    ObjectManager& mOm;
    RewardSS& mReward;
    CommonSS& mCommon;
};

}