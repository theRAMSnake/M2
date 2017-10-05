#pragma once
#include <Wt/WContainerWidget.h>
#include <messages/actions.pb.h>
#include "Common/MateriaServiceProxy.hpp"

class ActionsView : public Wt::WContainerWidget
{
public:
   ActionsView();

private:

   std::unique_ptr<MateriaServiceProxy<actions::ActionsService>> mService;
   actions::ActionsService_Stub* mActions;
};