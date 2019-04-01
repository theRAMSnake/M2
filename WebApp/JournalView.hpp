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
   void onSaveClick();
   void onIndexSelectionChanged();
   void onSearchClick();

   Wt::WTree* mIndexTree = nullptr;
   Wt::WTextEdit* mPageView = nullptr;
   Wt::WPushButton* mSaveBtn = nullptr;
   JournalModel& mModel;
};