#pragma once

#include "../ICore.hpp"
#include "Database.hpp"
#include "Strategy_v2.hpp"
#include "Backuper.hpp"
#include "Reward.hpp"

namespace materia
{

class Core : public ICore
{
public:
    Core(Database& db, const std::string& dbFileName);

    IBackuper& getBackuper() override;
    IStrategy_v2& getStrategy_v2() override;
    IReward& getReward() override;
    void onNewDay() override;
    void onNewWeek() override;

private:
    Database& mDb;
    Reward mReward;
    Strategy_v2 mStrategy_v2;
    Backuper mBackuper;
};

}