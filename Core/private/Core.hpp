#pragma once

#include "../ICore.hpp"

namespace materia
{

class Core : public ICore
{
public:
    IInbox& getInbox() const override;
    ICalendar& getCalendar() const override;
    IStrategy& getStrategy() const override;
    IJournal& getJournal() const override;
};

std::shared_ptr<ICore> createCore();

}