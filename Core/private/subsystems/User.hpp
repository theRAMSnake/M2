#pragma once
#include "../ISubsystem.hpp"

namespace materia
{

class ObjectManager;
class UserSS : public ISubsystem
{
public:
    UserSS(ObjectManager& objMan);
    void onNewDay() override;
    void onNewWeek() override;
    std::vector<TypeDef> getTypes() override;

private:
    void generateNewTOD();
    void awardInbox();

    ObjectManager& mOm;
};

}