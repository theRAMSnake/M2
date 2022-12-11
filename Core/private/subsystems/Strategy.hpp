
#pragma once
#include "../ISubsystem.hpp"
#include "Reward.hpp"

namespace materia
{

class ObjectManager;
class StrategySS : public ISubsystem
{
public:
    StrategySS(ObjectManager& objMan, RewardSS& reward);
    void onNewDay(const boost::gregorian::date& date) override;
    void onNewWeek() override;
    std::vector<TypeDef> getTypes() override;
    std::vector<CommandDef> getCommandDefs() override;

private:
    ObjectManager& mOm;
    RewardSS& mReward;
};

}
