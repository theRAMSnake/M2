#pragma once

#include <Wt/WContainerWidget.h>
#include "materia/JournalModel.hpp"

class JournalView : public Wt::WContainerWidget
{
public:
   JournalView(JournalModel& model);

private:
   Wt::WWidget* createIndexView();
   Wt::WWidget* createPageView();
   JournalModel& mModel;
};