#include "MainView.hpp"
#include <Wt/WGroupBox.h>
#include <Wt/WPushButton.h>
#include <Wt/WDialog.h>
#include "CalendarView.hpp"
#include "ActionsView.hpp"
#include "InboxView.hpp"
#include "materia/InboxModel.hpp"

MainView::MainView(CalendarModel& calendar, StrategyModel& strategy, InboxModel& inbox)
: mInbox(inbox)
{
   mInboxBtn = new Wt::WPushButton;
   mInboxBtn->setStyleClass("btn-primary btn-lg");
   mInboxBtn->clicked().connect(this, &MainView::showInbox);
   
   auto actionsGroup = new Wt::WGroupBox;
   actionsGroup->addStyleClass("col-md-10");
   actionsGroup->addWidget(std::unique_ptr<Wt::WPushButton>(mInboxBtn));

   auto calendarGroup = new Wt::WGroupBox;
   calendarGroup->addStyleClass("col-md-2");
   
   calendarGroup->addWidget(std::unique_ptr<Wt::WWidget>(new CalendarView(calendar)));
   actionsGroup->addWidget(std::unique_ptr<Wt::WWidget>(new ActionsView(strategy)));
   addWidget(std::unique_ptr<Wt::WGroupBox>(actionsGroup));
   addWidget(std::unique_ptr<Wt::WGroupBox>(calendarGroup));

   updateInboxBtnText();
}

void MainView::showInbox()
{
   Wt::WDialog* dialog = new Wt::WDialog("Inbox View");

   dialog->contents()->addWidget(std::unique_ptr<Wt::WWidget>(new InboxView(mInbox)));

   Wt::WPushButton *ok = new Wt::WPushButton("OK");
   dialog->footer()->addWidget(std::unique_ptr<Wt::WPushButton>(ok));
   ok->setDefault(true);

   ok->clicked().connect(std::bind([=]() {
      dialog->accept();
   }));

   dialog->finished().connect(std::bind([=]() {
         updateInboxBtnText();
         delete dialog;
   }));

   dialog->setWidth(750);
   dialog->setHeight(Wt::WLength("85%"));
   dialog->show();
}

void MainView::updateInboxBtnText()
{
   mInboxBtn->setText("Inbox (" + std::to_string(mInbox.get().size()) + ")");
}