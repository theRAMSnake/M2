#pragma once
#include "../ISubsystem.hpp"

namespace materia
{

class ObjectManager;
class JournalSS : public ISubsystem
{
public:
    JournalSS(ObjectManager& objMan);
    void onNewDay() override;
    void onNewWeek() override;
    std::vector<TypeDef> getTypes() override;

private:
    void handleJournalContentItemChange(Object& obj);
    void handleJournalContentDeleted(Object& obj);
    void handleJournalHeaderDeleted(Object& obj);
    void handleJournalContentCreated(Object& obj);

    ObjectManager& mOm;
};

}