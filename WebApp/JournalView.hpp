#pragma once

#include <Wt/WContainerWidget.h>
#include "materia/JournalModel.hpp"

class Dialog;
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
   void navigate(const materia::Id id, std::size_t pos);
   Wt::WTreeNode* findNodeById(Wt::WTreeNode* node, const materia::Id id);

   Wt::WTree* mIndexTree = nullptr;
   Wt::WTextEdit* mPageView = nullptr;
   Wt::WPushButton* mSaveBtn = nullptr;
   JournalModel& mModel;
   Dialog* mDlg;
};