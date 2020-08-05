#pragma once
#include "../ISubsystem.hpp"

namespace materia
{

class ObjectManager;
class FinanceSS : public ISubsystem
{
public:
    FinanceSS(ObjectManager& objMan);
    void onNewDay() override;
    void onNewWeek() override;
    std::vector<TypeDef> getTypes() override;

private:
    void performFinancialAnalisys();

    ObjectManager& mOm;
};

}