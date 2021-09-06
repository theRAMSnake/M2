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
    void setMod(const Id& id, const std::string& desc, const double value);
    void removeMod(const Id& id);

private:
    Command* parseRewardCommand(const boost::property_tree::ptree& src);
    void genContract();
    void levelUpContract(const Id id);
    double calculateTotalModifier();

    ObjectManager& mOm;
};

}
