#pragma once
#include "../ISubsystem.hpp"

namespace materia
{

class ObjectManager;
class CalendarSS : public ISubsystem
{
public:
    CalendarSS(ObjectManager& objMan);
    void onNewDay() override;
    void onNewWeek() override;
    std::vector<TypeDef> getTypes() override;

private:
    ObjectManager& mOm;
};

}