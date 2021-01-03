#pragma once
#include "../ISubsystem.hpp"

namespace materia
{

class ObjectManager;
class RewardSS : public ISubsystem
{
public:
    RewardSS(ObjectManager& objMan);

    void onNewDay(const boost::gregorian::date& date) override;
    void onNewWeek() override;
    
    std::vector<TypeDef> getTypes() override;
    std::vector<CommandDef> getCommandDefs() override;

    void addPoints(const int points);

private:
    Command* parseRewardCommand(const boost::property_tree::ptree& src);
    void genContract();
    void levelUpContract(const Id id);
    double calculateTotalModifier();

    double mLeftOver = 0.0;
    ObjectManager& mOm;
};

}