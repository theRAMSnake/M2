#pragma once
#include "../ISubsystem.hpp"

namespace materia
{

class ObjectManager;
class ChallengeSS : public ISubsystem
{
public:
    ChallengeSS(ObjectManager& objMan);
    void onNewDay() override;
    void onNewWeek() override;
    std::vector<TypeDef> getTypes() override;

private:
    void handleChItemChange(Object& obj);

    ObjectManager& mOm;
};

}