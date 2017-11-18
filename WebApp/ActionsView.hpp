#pragma once
#include <Wt/WContainerWidget.h>
#include <Wt/WCalendar.h>
#include <messages/actions.pb.h>
#include <messages/calendar.pb.h>
#include "Common/MateriaServiceProxy.hpp"

class ActionsView : public Wt::WContainerWidget
{
public:
   ActionsView();

private:
   std::unique_ptr<MateriaServiceProxy<actions::ActionsService>> mService;
   actions::ActionsService_Stub* mActions;

   std::unique_ptr<MateriaServiceProxy<calendar::CalendarService>> mCService;
   calendar::CalendarService_Stub* mCalendar;
};