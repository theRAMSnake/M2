#pragma once

#include "../ICore3.hpp"
#include "Core.hpp"

namespace materia
{

class Core3 : public ICore3
{
public:
    Core3(const CoreConfig& config);

    IInbox& getInbox() override;
    ICalendar& getCalendar() override;
    IJournal& getJournal() override;
    IBackuper& getBackuper() override;
    IStrategy_v2& getStrategy_v2() override;
    IFreeData& getFreeData() override;
    IFinance& getFinance() override;
    IReward& getReward() override;
    IChallenge& getChallenge() override;
    void onNewDay() override;
    void onNewWeek() override;

    std::string executeCommandJson(const std::string& json) override;

private:
    Database mDb;
    Core mOldCore;
};

}