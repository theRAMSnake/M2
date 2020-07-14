#pragma once

#include "../ICore3.hpp"
#include "Core.hpp"
#include "TypeSystem.hpp"
#include "ObjectManager.hpp"
#include "CommandParser.hpp"

namespace materia
{

class Core3 : public ICore3
{
public:
    Core3(const CoreConfig& config);

    IBackuper& getBackuper() override;
    IStrategy_v2& getStrategy_v2() override;
    IReward& getReward() override;
    void onNewDay() override;
    void onNewWeek() override;

    std::string executeCommandJson(const std::string& json) override;

private:
    std::string formatResponce(const ExecutionResult& result);
    std::string formatErrorResponce(const std::string& errorText);
    void notifyLongCommand(const std::string& cmd, unsigned int value);

    void generateNewTOD();

    Database mDb;
    TypeSystem mTypeSystem;
    Core mOldCore;
    ObjectManager mObjManager;
};

}