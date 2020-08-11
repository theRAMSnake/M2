#pragma once

#include "../ICore.hpp"
#include "Database.hpp"
#include "Strategy_v2.hpp"
#include "Backuper.hpp"

namespace materia
{

class Core : public ICore
{
public:
    Core(Database& db, const std::string& dbFileName);

    IBackuper& getBackuper() override;
    IStrategy_v2& getStrategy_v2() override;
    void onNewDay() override;
    void onNewWeek() override;

private:
    Database& mDb;
    Strategy_v2 mStrategy_v2;
    Backuper mBackuper;
};

}