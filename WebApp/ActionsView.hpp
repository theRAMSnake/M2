#pragma once
#include <Wt/WContainerWidget>
#include <messages/actions.pb.h>
#include "MateriaServiceProvider.hpp"

class ActionsView : public Wt::WContainerWidget
{
public:
   ActionsView();

private:

   std::unique_ptr<MateriaServiceProvider<actions::ActionsService>> mService;
   actions::ActionsService_Stub* mActions;
};