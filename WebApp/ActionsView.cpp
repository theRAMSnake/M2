#include "ActionsView.hpp"
#include "CommonDialogManager.hpp"
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

class ActionItemViewEditDialog : public Wt::WDialog
{
public:
   typedef std::function<void(const std::string&, const std::string&)> TOnOkCallback;
   ActionItemViewEditDialog(const std::string& title, const std::string& desc, TOnOkCallback cb)
   {
      setWidth(Wt::WLength("75%"));
      mTitle = new Wt::WLineEdit(title);
      contents()->addWidget(std::unique_ptr<Wt::WLineEdit>(mTitle));

      mDesc = new Wt::WTextEdit();
      mDesc->setHeight(500);
      mDesc->setConfigurationSetting("branding", false);
      mDesc->setConfigurationSetting("elementpath", false);
      mDesc->setConfigurationSetting("Browser_spellcheck", true);
      mDesc->setConfigurationSetting("statusbar", false);
      mDesc->setConfigurationSetting("menubar", "edit format table");
      
      mDesc->setExtraPlugins("colorpicker, textcolor, searchreplace, table, lists");
      mDesc->setToolBar(0, "bold italic | link | forecolor backcolor | fontsizeselect | numlist bullist");
      mDesc->setText(desc);
      
      contents()->addWidget(std::unique_ptr<Wt::WTextEdit>(mDesc));

      auto ok = new Wt::WPushButton("Accept");
      ok->setDefault(true);
      ok->clicked().connect(std::bind([=]() {
        accept();
      }));
      footer()->addWidget(std::unique_ptr<Wt::WPushButton>(ok));

      auto cancel = new Wt::WPushButton("Cancel");
      footer()->addWidget(std::unique_ptr<Wt::WPushButton>(cancel));
      cancel->clicked().connect(this, &Wt::WDialog::reject);

      rejectWhenEscapePressed();

      finished().connect(std::bind([=]() {
        if (result() == Wt::DialogCode::Accepted)
        {
            cb(mTitle->text().narrow(), mDesc->text().narrow());
        }

        delete this;
      }));
   }

private:
   Wt::WLineEdit* mTitle;
   Wt::WTextEdit* mDesc;
};

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
      auto dlg = new ActionItemViewEditDialog(
         mTask.title,
         mTask.notes,
         std::bind(&ActionItemView::onDialogOk, this, std::placeholders::_1, std::placeholders::_2));
      dlg->show();
   }

   void onDialogOk(const std::string& title, const std::string& desc)
   {
      mLabel->setText(title);

      mTask.title = title;
      mTask.notes = desc;

      mStrategy.modifyTask(mTask);
   }

   void onClick(Wt::WMouseEvent ev)
   {
      if(ev.modifiers().test(Wt::KeyboardModifier::Control))
      {
          std::function<void()> elementDeletedFunc = [=] () {
            mStrategy.deleteTask(mTask.id);
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
   //addStyleClass("container-fluid");

   //auto actionsGroup = new Wt::WContainerWidget;
   //actionsGroup->addStyleClass("col-md-10");

   for(auto x : mStrategy.getActiveTasks())
   {
      /*actionsGroup->*/addWidget(std::unique_ptr<Wt::WContainerWidget>(new ActionItemView(x, mStrategy)));
   }
   
   //addWidget(std::unique_ptr<Wt::WContainerWidget>(actionsGroup));
}