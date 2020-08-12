#pragma once
#include "../ISubsystem.hpp"

namespace materia
{

class ObjectManager;
class RewardSS : public ISubsystem
{
public:
    RewardSS(ObjectManager& objMan);

    void onNewDay() override;
    void onNewWeek() override;
    
    std::vector<TypeDef> getTypes() override;
    std::vector<CommandDef> getCommandDefs() override;

    void addPoints(const int points);

private:
    Command* parseRewardCommand(const boost::property_tree::ptree& src);

    ObjectManager& mOm;
};

}