#include "Common.hpp"
#include "../ObjectManager.hpp"
#include "../types/SimpleList.hpp"
#include "../ExceptionsUtil.hpp"

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
    return {{"push", std::bind(&CommonSS::parsePushCommand, this, std::placeholders::_1)}};
}

void CommonSS::push(const Id listId, const std::string& value)
{
    types::SimpleList lst(mOm, listId);
    lst.add(value);
}

class PushCommand : public Command
{
public:
   PushCommand(const Id listId, const std::string& value, CommonSS& common)
   : mId(listId)
   , mValue(value)
   , mCommon(common)
   {

   }

   ExecutionResult execute(ObjectManager& objManager) override
   {
      mCommon.push(mId, mValue);
      return Success{};
   }

private:
    const Id mId;
    const std::string mValue;
    CommonSS& mCommon;
};

Command* CommonSS::parsePushCommand(const boost::property_tree::ptree& src)
{
   auto id = getOrThrow<std::string>(src, "listId", "List id is not specified");
   auto val = getOrThrow<std::string>(src, "value", "Value is not specified");

   return new PushCommand(id, val, *this);
}

}