#pragma once
#include "../ISubsystem.hpp"

namespace materia
{

class ObjectManager;
class CalendarSS : public ISubsystem
{
public:
    CalendarSS(ObjectManager& objMan);
    void onNewDay(const boost::gregorian::date& date) override;
    void onNewWeek() override;
    std::vector<TypeDef> getTypes() override;
    std::vector<CommandDef> getCommandDefs() override;

private:
    ObjectManager& mOm;
};

}