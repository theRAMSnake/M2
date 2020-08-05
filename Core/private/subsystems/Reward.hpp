#pragma once
#include "../ISubsystem.hpp"

namespace materia
{

class ObjectManager;
class RewardSS : public ISubsystem
{
public:
    RewardSS(ObjectManager& objMan);

    void onNewDay() override;
    void onNewWeek() override;
    
    std::vector<TypeDef> getTypes() override;

private:
    ObjectManager& mOm;
};

}