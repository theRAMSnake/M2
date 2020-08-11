#include "User.hpp"
#include "../ObjectManager.hpp"
#include "../types/SimpleList.hpp"
#include "../types/Variable.hpp"
#include "../ExceptionsUtil.hpp"
#include <boost/date_time/gregorian/greg_date.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace materia
{

Time advance(const Time src, const int recType)
{
    Time result = src;

    switch (recType)
    {
    case 1:
        result.value += 604800;
        break;

    case 2:
        result.value = boost::posix_time::to_time_t(boost::posix_time::from_time_t(result.value) + boost::gregorian::months(1));
        break;

    case 3:
        result.value = boost::posix_time::to_time_t(boost::posix_time::from_time_t(result.value) + boost::gregorian::months(3));
        break;

    case 4:
        result.value = boost::posix_time::to_time_t(boost::posix_time::from_time_t(result.value) + boost::gregorian::months(12));
        break;
    
    default:
        break;
    }

    return result;
}

void complete(const Id id, ObjectManager& om, RewardSS& reward)
{
    auto objectPtr = om.get(id);
    auto object = *objectPtr;

    if(object.getType().name != "calendar_item")
    {
        throw std::runtime_error(fmt::format("Type {} is not completable", object.getType().name));
    }

    auto eType = object["entityType"].get<Type::Option>();
    auto recType = object["reccurencyType"].get<Type::Option>();
    if(eType == 1/*task*/)
    {
        reward.addPoints(1);
    }

    if(recType != 0/*none*/)
    {
        object["timestamp"] = advance(object["timestamp"].get<Type::Timestamp>(), recType);
        om.modify(object);
    }
    else
    {
        om.destroy(id);
    }
}

class CompleteCommand : public Command
{
public:
   CompleteCommand(const Id& id, RewardSS& reward)
   : mId(id)
   , mReward(reward)
   {

   }

   ExecutionResult execute(ObjectManager& objManager) override
   {
      complete(mId, objManager, mReward);
      return Success{};
   }

private:
    const Id mId;
    RewardSS& mReward;
};

Command* UserSS::parseComplete(const boost::property_tree::ptree& src)
{
   auto id = getOrThrow<Id>(src, "id", "Id is not specified");

   return new CompleteCommand(id, mReward);
}

UserSS::UserSS(ObjectManager& objMan, RewardSS& reward)
: mOm(objMan)
, mReward(reward)
{

}

void UserSS::generateNewTOD()
{
   types::SimpleList wisdom(mOm, Id("wisdom"));
   types::Variable tod(mOm, Id("tip_of_the_day"));
   if(wisdom.size() > 0)
   {
      auto pos = rand() % wisdom.size();
      tod = wisdom.at(pos);
   }
}

void UserSS::awardInbox()
{
   types::SimpleList inbox(mOm, Id("inbox"));
   if(inbox.size() == 0 && rand() % 10 == 0)
   {
      mReward.addPoints(1);
      inbox.add("Extra point awarded for empty inbox.");
   }
}

void UserSS::onNewDay()
{
   awardInbox(); 
   generateNewTOD();
}

void UserSS::onNewWeek()
{
    
}

std::vector<TypeDef> UserSS::getTypes()
{
   return {};
}

std::vector<CommandDef> UserSS::getCommandDefs()
{
    return {{"complete", std::bind(&UserSS::parseComplete, this, std::placeholders::_1)}};
}

}

