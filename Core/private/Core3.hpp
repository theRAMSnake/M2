#pragma once

#include "../ICore3.hpp"
#include "TypeSystem.hpp"
#include "ObjectManager.hpp"
#include "CommandParser.hpp"
#include "ISubsystem.hpp"

namespace materia
{

class CommonSS;
class Core3 : public ICore3
{
public:
    Core3(const CoreConfig& config);

    void onNewDay(const boost::gregorian::date& date) override;
    void onNewWeek() override;

    std::string executeCommandJson(const std::string& json) override;

private:
    std::string formatResponce(const ExecutionResult& result);
    std::string formatErrorResponce(const std::string& errorText);
    void notifyLongCommand(const std::string& cmd, unsigned int value);

    void generateNewTOD();

    Database mDb;
    TypeSystem mTypeSystem;
    ObjectManager mObjManager;
    CommonSS* mCommonSS;
    std::vector<CommandDef> mCommandDefs;
    std::vector<std::shared_ptr<ISubsystem>> mSubsystems;
};

}