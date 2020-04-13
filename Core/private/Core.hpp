#pragma once

#include "../ICore.hpp"
#include "Database.hpp"
#include "Strategy_v2.hpp"
#include "Journal.hpp"
#include "Inbox.hpp"
#include "Calendar.hpp"
#include "Backuper.hpp"
#include "FreeData.hpp"
#include "Finance.hpp"
#include "Reward.hpp"
#include "Challenge.hpp"

namespace materia
{

class Core : public ICore
{
public:
    Core(Database& db, const std::string& dbFileName);

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

private:
    Database& mDb;
    Inbox mInbox;
    Reward mReward;
    Calendar mCalendar;
    Journal mJournal;
    freedata::FreeData mFreeData;
    Challenge mChallenge;
    Strategy_v2 mStrategy_v2;
    Backuper mBackuper;
    Finance mFinance;
};

}