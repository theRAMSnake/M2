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

    void addCoins(const int coins, const std::string& color);
    void setModAndGenerator(const Id& id, const std::string& desc, const double value, const int valueInt, const std::optional<std::string>& color);
    void removeModAndGenerator(const Id& id);
    Object useChest();

private:
    Command* parseUseChestCommand(const boost::property_tree::ptree& src);
    Command* parseRewardCommand(const boost::property_tree::ptree& src);
    void genContract();
    void levelUpContract(const Id id);
    double calculateTotalModifier();

    ObjectManager& mOm;
};

}
