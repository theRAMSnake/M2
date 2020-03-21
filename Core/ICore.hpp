#pragma once

#include <memory>

namespace materia
{

class IInbox;
class ICalendar;
class IStrategy;
class IStrategy_v2;
class IJournal;
class IBackuper;
class IFreeData;
class IFinance;
class IReward;

struct CoreConfig
{
    std::string dbFileName;
};

class ICore
{
public:
    virtual IInbox& getInbox() = 0;
    virtual ICalendar& getCalendar() = 0;
    virtual IStrategy_v2& getStrategy_v2() = 0;
    virtual IJournal& getJournal() = 0;
    virtual IBackuper& getBackuper() = 0;
    virtual IFreeData& getFreeData() = 0;
    virtual IFinance& getFinance() = 0;
    virtual IReward& getReward() = 0;

    virtual void onNewDay() = 0;

    virtual ~ICore(){}
};

std::shared_ptr<ICore> createCore(const CoreConfig& config);

}