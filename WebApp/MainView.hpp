#pragma once

#include <Wt/WContainerWidget.h>

class CalendarModel;
class FinanceModel;
class InboxModel;
class JournalModel;
class MainView : public Wt::WContainerWidget
{
public:
   MainView(CalendarModel& calendar, FinanceModel& finance, InboxModel& inbox, JournalModel& journal);

private:
   void showInbox();
   void updateInboxBtnText();

   Wt::WPushButton* mInboxBtn;
   InboxModel& mInbox;
};