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

namespace materia
{

class Core : public ICore
{
public:
    Core(const CoreConfig& config);

    IInbox& getInbox() override;
    ICalendar& getCalendar() override;
    IJournal& getJournal() override;
    IBackuper& getBackuper() override;
    IStrategy_v2& getStrategy_v2() override;
    IFreeData& getFreeData() override;
    IFinance& getFinance() override;

private:
    Database mDb;
    Inbox mInbox;
    Calendar mCalendar;
    Journal mJournal;
    freedata::FreeData mFreeData;
    Strategy_v2 mStrategy_v2;
    Backuper mBackuper;
    Finance mFinance;
};

std::shared_ptr<ICore> createCore(const CoreConfig& config);

}