#pragma once
#include "../ISubsystem.hpp"

namespace materia
{

class ObjectManager;
class IdeasSS : public ISubsystem
{
public:
    IdeasSS(ObjectManager& objMan);
    void onNewDay(const boost::gregorian::date& date) override;
    void onNewWeek() override;
    std::vector<TypeDef> getTypes() override;
    std::vector<CommandDef> getCommandDefs() override;

private:
    Command* parseGetRelatedIdeas(const boost::property_tree::ptree& src);
    Command* parseSearchIdeas(const boost::property_tree::ptree& src);

    ObjectManager& mOm;
};

}