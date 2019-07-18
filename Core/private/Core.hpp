#pragma once

#include "../ICore.hpp"
#include "Database.hpp"
#include "Strategy.hpp"
#include "Strategy_v2.hpp"
#include "Journal.hpp"
#include "Inbox.hpp"
#include "Calendar.hpp"
#include "Backuper.hpp"
#include "FreeData.hpp"

namespace materia
{

class Core : public ICore
{
public:
    Core(const CoreConfig& config);

    IInbox& getInbox() override;
    ICalendar& getCalendar() override;
    IStrategy& getStrategy() override;
    IJournal& getJournal() override;
    IBackuper& getBackuper() override;
    IStrategy_v2& getStrategy_v2() override;
    IFreeData& getFreeData() override;

private:
    Database mDb;
    Inbox mInbox;
    Calendar mCalendar;
    Journal mJournal;
    Strategy mStrategy;
    Strategy_v2 mStrategy_v2;
    Backuper mBackuper;
    freedata::FreeData mFreeData;
};

std::shared_ptr<ICore> createCore(const CoreConfig& config);

}