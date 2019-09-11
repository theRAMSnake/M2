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
   void onMoveClick();
   void onRandomClick();
   void onSharedPageChecked();
   void onSharedPageUnchecked();
   void navigate(const materia::Id id, std::size_t pos);
   Wt::WTreeNode* findNodeById(Wt::WTreeNode* node, const materia::Id id);

   bool isPageShared(const std::string& title);
   void saveSharedPage(const std::string& title, const std::string& content);
   void eraseSharedPage(const std::string& title) const;

   Wt::WTree* mIndexTree = nullptr;
   Wt::WTextEdit* mPageView = nullptr;
   Wt::WPushButton* mSaveBtn = nullptr;
   Wt::WCheckBox* mIsSharedCheckbox = nullptr;
   JournalModel& mModel;
   Dialog* mDlg;
};