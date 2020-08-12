#pragma once
#include "../ISubsystem.hpp"

namespace materia
{

class ObjectManager;
class CommonSS : public ISubsystem
{
public:
    CommonSS(ObjectManager& objMan);
    void onNewDay() override;
    void onNewWeek() override;
    std::vector<TypeDef> getTypes() override;
    std::vector<CommandDef> getCommandDefs() override;

private:
    ObjectManager& mOm;
};

}