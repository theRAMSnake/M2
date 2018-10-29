#pragma once

#include <Wt/WContainerWidget.h>

class CalendarModel;
class StrategyModel;
class InboxModel;
class MainView : public Wt::WContainerWidget
{
public:
   MainView(CalendarModel& calendar, StrategyModel& strategy, InboxModel& inbox);

private:
   void showInbox();
   void updateInboxBtnText();

   Wt::WPushButton* mInboxBtn;
   InboxModel& mInbox;
};