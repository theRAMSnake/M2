#include "FinanceView.hpp"
#include <Wt/WGroupBox.h>
#include <Wt/WPushButton.h>
#include <Wt/WDialog.h>
#include <Wt/WLabel.h>
#include "materia/FinanceModel.hpp"
#include "dialog/Dialog.hpp"

class EventsView : public Wt::WContainerWidget
{
public:
   EventsView(FinanceModel& model)
   {

   }
};

class CategoriesView : public Wt::WContainerWidget
{
public:
   CategoriesView(FinanceModel& model)
   {

   }
};


FinanceView::FinanceView(FinanceModel& model)
: mFinance(model)
{
   mCategoriesBtn = new Wt::WPushButton("Categories");
   mCategoriesBtn->setStyleClass("btn-primary btn-lg");
   mCategoriesBtn->clicked().connect(this, &FinanceView::showCategories);
   
   auto eventsGroup = new Wt::WGroupBox;
   eventsGroup->addStyleClass("col-md-10");
   eventsGroup->addWidget(std::unique_ptr<Wt::WPushButton>(mCategoriesBtn));

   eventsGroup->addWidget(std::unique_ptr<Wt::WWidget>(new EventsView(model)));
   addWidget(std::unique_ptr<Wt::WGroupBox>(eventsGroup));
}

void FinanceView::showCategories()
{
   Dialog* dlg = new Dialog("Categories View", std::make_unique<CategoriesView>(mFinance));
   dlg->show();
}