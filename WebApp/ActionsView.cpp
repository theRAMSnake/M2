#include "ActionsView.hpp"
#include "dialog/CommonDialogManager.hpp"
#include "TaskEditDialog.hpp"
#include <Wt/WTree.h>
#include <Wt/WTreeNode.h>
#include <Wt/WCssDecorationStyle.h>
#include <Wt/WTextEdit.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WDialog.h>
#include <Wt/WCheckBox.h>
#include <Wt/WGroupBox.h>
#include <Wt/WCalendar.h>
#include <Wt/WTimeEdit.h>
#include <Wt/WDateEdit.h>
#include <Wt/WLabel.h>
#include "boost/date_time/gregorian/gregorian.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>

class ActionItemView : public Wt::WContainerWidget
{
public:
   ActionItemView(const StrategyModel::Task& task, StrategyModel& strategy)
   : mTask(task)
   , mStrategy(strategy)
   {
      mLabel = addWidget(std::unique_ptr<Wt::WLabel>(new Wt::WLabel(task.title)));
      setStyleClass("Task");
      doubleClicked().connect(std::bind(&ActionItemView::onDblClicked, this, std::placeholders::_1));
      clicked().connect(std::bind(&ActionItemView::onClick, this, std::placeholders::_1));
   }

private:
   void onDblClicked(Wt::WMouseEvent ev)
   {
      std::function<void()> func = [=] () {
         mStrategy.completeTask(mTask);
         };

      CommonDialogManager::showConfirmationDialog("Complete it?", func);
   }

   void onClick(Wt::WMouseEvent ev)
   {
      if(ev.modifiers().test(Wt::KeyboardModifier::Control))
      {
         std::function<void()> elementDeletedFunc = [=] () {
            mStrategy.deleteTask(mTask);
         };

         CommonDialogManager::showConfirmationDialog("Delete it?", elementDeletedFunc);
      }
   }

   Wt::WLabel* mLabel;
   StrategyModel::Task mTask;
   StrategyModel& mStrategy;
};

ActionsView::ActionsView(StrategyModel& strategy)
: mStrategy(strategy)
{

   for(auto x : mStrategy.getActiveTasks())
   {
      addWidget(std::unique_ptr<Wt::WContainerWidget>(new ActionItemView(x, mStrategy)));
   }
}