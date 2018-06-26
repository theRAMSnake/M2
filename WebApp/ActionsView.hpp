#pragma once
#include <Wt/WContainerWidget.h>
#include <Wt/WCalendar.h>
#include <Client/IActions.hpp>

class ActionsView : public Wt::WContainerWidget
{
public:
   ActionsView(materia::IActions& actions);

private:
   void initiateItemAdd();
   materia::IActions& mActions;
};