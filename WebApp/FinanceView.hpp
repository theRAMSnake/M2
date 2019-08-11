#pragma once

#include <Wt/WContainerWidget.h>

class FinanceModel;
class FinanceView : public Wt::WContainerWidget
{
public:
   FinanceView(FinanceModel& model);

private:
   void showCategories();

   Wt::WPushButton* mCategoriesBtn;
   FinanceModel& mFinance;
};