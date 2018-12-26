#pragma once

#include <memory>

namespace materia
{

class IInbox;
class ICalendar;
class IStrategy;
class IJournal;
class IBackuper;

struct CoreConfig
{
    std::string dbFileName;
};

class ICore
{
public:
    virtual IInbox& getInbox() = 0;
    virtual ICalendar& getCalendar() = 0;
    virtual IStrategy& getStrategy() = 0;
    virtual IJournal& getJournal() = 0;
    virtual IBackuper& getBackuper() = 0;

    virtual ~ICore(){}
};

std::shared_ptr<ICore> createCore(const CoreConfig& config);

}