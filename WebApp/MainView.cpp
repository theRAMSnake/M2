#include "MainView.hpp"
#include <Wt/WGroupBox.h>
#include <Wt/WPushButton.h>
#include <Wt/WDialog.h>
#include <Wt/WLabel.h>
#include "CalendarView.hpp"
#include "ActionsView.hpp"
#include "InboxView.hpp"
#include "JournalView.hpp"
#include "materia/InboxModel.hpp"
#include "dialog/Dialog.hpp"

MainView::MainView(CalendarModel& calendar, StrategyModel& strategy, InboxModel& inbox, JournalModel& journal)
: mInbox(inbox)
{
   auto tipOfTheDayLabel = new Wt::WLabel(journal.getTipOfTheDay());
   tipOfTheDayLabel->setStyleClass("tod");
   addWidget(std::unique_ptr<Wt::WLabel>(tipOfTheDayLabel));

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
   Dialog* dlg = new Dialog("Inbox View", std::make_unique<InboxView>(mInbox));
   dlg->OnFinished.connect(std::bind([=]() {
         updateInboxBtnText();
   }));
   dlg->show();
}

void MainView::updateInboxBtnText()
{
   mInboxBtn->setText("Inbox (" + std::to_string(mInbox.get().size()) + ")");
}