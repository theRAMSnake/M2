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

    void spawnShopItems();

    std::vector<TypeDef> getTypes() override;
    std::vector<CommandDef> getCommandDefs() override;

    void addCoins(const int coins, const std::string& color);
    void setGenerator(const Id& id, const std::string& desc, const int valueInt, const std::optional<std::string>& color);
    void removeGenerator(const Id& id);
    void buyRewardItem(const Id id);

private:
    Command* parseRewardCommand(const boost::property_tree::ptree& src);
    Command* parseBuyCommand(const boost::property_tree::ptree& src);
    void genContract();
    void levelUpContract(const Id id);

    ObjectManager& mOm;
};

}
