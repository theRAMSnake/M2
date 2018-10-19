#pragma once

#include <memory>

namespace materia
{

class IInbox;
class ICalendar;
class IStrategy;
class IJournal;
class ICore
{
public:
    virtual IInbox& getInbox() const = 0;
    virtual ICalendar& getCalendar() const = 0;
    virtual IStrategy& getStrategy() const = 0;
    virtual IJournal& getJournal() const = 0;

    virtual ~ICore(){}
};

std::shared_ptr<ICore> createCore();

}