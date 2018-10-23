#pragma once

#include "../ICore.hpp"
#include "Database.hpp"
#include "Strategy.hpp"
#include "Journal.hpp"
#include "Inbox.hpp"
#include "Calendar.hpp"

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

private:
    Database mDb;
    Inbox mInbox;
    Calendar mCalendar;
    Journal mJournal;
    Strategy mStrategy;
};

std::shared_ptr<ICore> createCore(const CoreConfig& config);

}