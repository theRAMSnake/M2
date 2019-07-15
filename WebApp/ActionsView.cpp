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
   ActionItemView(const std::vector<StrategyModel::Task>& tasks, StrategyModel& strategy)
   : mTasks(tasks)
   , mStrategy(strategy)
   {
      auto title = tasks[0].title;
      if(tasks.size() > 1)
      {
         title += " (" + std::to_string(tasks.size()) + " left)";
      }

      mLabel = addWidget(std::unique_ptr<Wt::WLabel>(new Wt::WLabel(title)));
      setStyleClass("Task");
      doubleClicked().connect(std::bind(&ActionItemView::onDblClicked, this, std::placeholders::_1));
      clicked().connect(std::bind(&ActionItemView::onClick, this, std::placeholders::_1));
   }

private:
   void onDblClicked(Wt::WMouseEvent ev)
   {
      std::function<void()> func = [=] () {
         mStrategy.completeTask(mTasks.back());
         updateView();
      };

      CommonDialogManager::showConfirmationDialog("Complete it?", func);
   }

   void onClick(Wt::WMouseEvent ev)
   {
      if(ev.modifiers().test(Wt::KeyboardModifier::Control))
      {
         std::function<void()> elementDeletedFunc = [=] () {
            mStrategy.deleteTask(mTasks.back());
            updateView();
         };

         CommonDialogManager::showConfirmationDialog("Delete it?", elementDeletedFunc);
      }
   }

   void updateView()
   {
      mTasks.pop_back();

      if(mTasks.empty())
      {
         parent()->removeChild(this);
      }
      else
      {
         auto title = mTasks[0].title;
         if(mTasks.size() > 1)
         {
            title += " (" + std::to_string(mTasks.size()) + " left)";
         }

         mLabel->setText(title);
      }
   }

   Wt::WLabel* mLabel;
   std::vector<StrategyModel::Task> mTasks;
   StrategyModel& mStrategy;
};

ActionsView::ActionsView(StrategyModel& strategy)
: mStrategy(strategy)
{
   auto tasks = mStrategy.getActiveTasks();

   std::map<materia::Id, std::vector<StrategyModel::Task>> mergedTasks;
   for(auto x : tasks)
   {
      mergedTasks[x.nodeId].push_back(x);
   }
   
   for(auto x : mergedTasks)
   {
      addWidget(std::unique_ptr<Wt::WContainerWidget>(new ActionItemView(x.second, mStrategy)));
   }
}