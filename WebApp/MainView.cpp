#include "MainView.hpp"
#include <Wt/WGroupBox.h>
#include <Wt/WPushButton.h>
#include <Wt/WDialog.h>
#include <Wt/WLabel.h>
#include "CalendarView.hpp"
#include "InboxView.hpp"
#include "materia/FinanceModel.hpp"
#include "JournalView.hpp"
#include "materia/InboxModel.hpp"
#include "dialog/Dialog.hpp"
#include "WtConverters.hpp"

std::string statusToHtml(const finance::FinanceStatus s)
{
   switch(s)
   {
      case finance::FinanceStatus::CRITICAL:
         return "<font color=\"red\">Critical</font>";

      case finance::FinanceStatus::OK:
         return "<font color=\"black\">OK</font>";

      case finance::FinanceStatus::GOOD:
         return "<font color=\"darkgreen\">Good</font>";

      case finance::FinanceStatus::GREAT:
         return "<font color=\"darkgreen\">Great!</font>";

      case finance::FinanceStatus::EXCELLENT:
         return "<font color=\"darkgreen\">Excellent, Snake!</font>";

      default:
         return "Error";
   }
}

std::string reportToHtml(const FinanceModel::Report& r)
{
   return "<p> Finance status: " + statusToHtml(r.status) + ", balance: " + currencyToString(r.balance) +  "</p>";
}

MainView::MainView(CalendarModel& calendar, FinanceModel& finance, InboxModel& inbox, JournalModel& journal)
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

   actionsGroup->addWidget(std::make_unique<Wt::WLabel>(reportToHtml(finance.getReport())));

   auto calendarGroup = new Wt::WGroupBox;
   calendarGroup->addStyleClass("col-md-2");
   
   calendarGroup->addWidget(std::unique_ptr<Wt::WWidget>(new CalendarView(calendar)));
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