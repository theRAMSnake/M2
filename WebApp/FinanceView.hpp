#pragma once

#include <Wt/WContainerWidget.h>

class FinanceModel;
class FinanceView : public Wt::WContainerWidget
{
public:
   FinanceView(FinanceModel& model);

private:
   FinanceModel& mFinance;
};