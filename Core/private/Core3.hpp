#pragma once

#include "../ICore3.hpp"
#include "TypeSystem.hpp"
#include "ObjectManager.hpp"
#include "CommandParser.hpp"
#include "Connections.hpp"
#include "ISubsystem.hpp"

namespace materia
{

class CommonSS;
class RewardSS;
class Core3 : public ICore3
{
public:
    Core3(const CoreConfig& config);

    void onNewDay(const boost::gregorian::date& date) override;
    void onNewWeek() override;
    void healthcheck() override;

    std::string executeCommandJson(const std::string& json) override;

    void TEST_reinitReward() override;

private:
    std::string formatResponce(const ExecutionResult& result);
    std::string formatErrorResponce(const std::string& errorText);
    void notifyLongCommand(const std::string& cmd, unsigned int value);

    void generateNewTOD();

    Database mDb;
    TypeSystem mTypeSystem;
    Connections mConnections;
    ObjectManager mObjManager;
    CommonSS* mCommonSS;
    RewardSS* mRewardSS;
    std::vector<CommandDef> mCommandDefs;
    std::vector<std::shared_ptr<ISubsystem>> mSubsystems;
};

}
