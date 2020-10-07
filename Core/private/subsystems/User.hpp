#pragma once
#include "../ISubsystem.hpp"
#include "Reward.hpp"

namespace materia
{

class ObjectManager;
class StrategySS;
class UserSS : public ISubsystem
{
public:
    UserSS(ObjectManager& objMan, RewardSS& reward, StrategySS& strategy);
    void onNewDay() override;
    void onNewWeek() override;
    std::vector<TypeDef> getTypes() override;
    std::vector<CommandDef> getCommandDefs() override;

private:
    void generateNewTOD();
    void awardInbox();

    Command* parseComplete(const boost::property_tree::ptree& src);

    ObjectManager& mOm;
    RewardSS& mReward;
    StrategySS& mStrategy;
};

}