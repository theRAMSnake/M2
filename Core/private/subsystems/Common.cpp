#include "Common.hpp"
#include "../ObjectManager.hpp"

namespace materia
{

CommonSS::CommonSS(ObjectManager& objMan)
: mOm(objMan)
{

}

void CommonSS::onNewDay()
{

}

void CommonSS::onNewWeek()
{
   
}

std::vector<TypeDef> CommonSS::getTypes()
{
    std::vector<TypeDef> result;

    result.push_back({"object", "objects"});
    result.push_back({"variable", "variables", {{"value", Type::String}}});
    result.push_back({"simple_list", "lists", {{"objects", Type::StringArray}}});

    return result;
}

std::vector<CommandDef> CommonSS::getCommandDefs()
{
    return {};
}

}