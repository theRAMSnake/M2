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

    void push(const Id listId, const std::string& value);

private:
    Command* parsePushCommand(const boost::property_tree::ptree& src);
    ObjectManager& mOm;
};

}