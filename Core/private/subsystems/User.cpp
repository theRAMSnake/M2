#include "User.hpp"
#include "../ObjectManager.hpp"
#include "../types/SimpleList.hpp"
#include "../types/Variable.hpp"

namespace materia
{

UserSS::UserSS(ObjectManager& objMan)
: mOm(objMan)
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
      mOm.LEGACY_getReward().addPoints(1);
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

}

