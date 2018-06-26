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

class ActionItemView : public Wt::WLabel
{
public:
   ActionItemView(const materia::ActionItem& actionItem, materia::IActions& actions)
   : Wt::WLabel(actionItem.title)
   , mActionItem(actionItem)
   , mActions(actions)
   {
      doubleClicked().connect(std::bind(&ActionItemView::onDblClicked, this, std::placeholders::_1));
      clicked().connect(std::bind(&ActionItemView::onClick, this, std::placeholders::_1));
   }

private:
   void onDblClicked(Wt::WMouseEvent ev)
   {
      auto dlg = new ActionItemViewEditDialog(
         mActionItem.title,
         mActionItem.description,
         std::bind(&ActionItemView::onDialogOk, this, std::placeholders::_1, std::placeholders::_2));
      dlg->show();
   }

   void onDialogOk(const std::string& title, const std::string& desc)
   {
      setText(title);

      mActionItem.title = title;
      mActionItem.description = desc;

      mActions.replaceItem(mActionItem);
   }

   void onClick(Wt::WMouseEvent ev)
   {
      if(ev.modifiers().test(Wt::KeyboardModifier::Control))
      {
          std::function<void()> elementDeletedFunc = [=] () {
            mActions.deleteItem(mActionItem.id);
            };

          CommonDialogManager::showConfirmationDialog("Delete it?", elementDeletedFunc);
      }
   }

   materia::ActionItem mActionItem;
   materia::IActions& mActions;
};

ActionsView::ActionsView(materia::IActions& actions)
: mActions(actions)
{
   addStyleClass("container-fluid");

   auto actionsGroup = new Wt::WGroupBox;
   actionsGroup->addStyleClass("col-md-10");

   for(auto x : mActions.getItems())
   {
      actionsGroup->addWidget(std::unique_ptr<Wt::WCheckBox>(new ActionItemView(x, mActions)));
   }
   
   addWidget(std::unique_ptr<Wt::WGroupBox>(actionsGroup));
}

void ActionsView::initiateItemAdd()
{
   auto dlg = new ActionItemViewEditDialog(
   "",
   "",
   [=](const std::string& title, const std::string& desc)
   {
      materia::ActionItem newItem;
      newItem.title = title;
      newItem.description = desc;

      newItem.id = mActions.addItem(newItem);
      addChildNode(Wt::cpp14::make_unique<ActionItemView>(newItem, mActionsService));
   });
   dlg->show();
}