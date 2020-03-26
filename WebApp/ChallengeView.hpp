#pragma once
#include <Wt/WContainerWidget.h>
#include "materia/ChallengeModel.hpp"

class ChallengeView : public Wt::WContainerWidget
{
public:
   ChallengeView(ChallengeModel& model);

private:
   void onAddClick();
   void refreshList();

   Wt::WTable* mTable = nullptr;
   ChallengeModel& mModel;
};