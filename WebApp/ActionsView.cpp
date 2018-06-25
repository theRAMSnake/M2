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

class ActionItemView : public Wt::WTreeNode
{
public:
   ActionItemView(const actions::ActionInfo& actionInfo, actions::ActionsService_Stub& actionsService)
   : Wt::WTreeNode(actionInfo.title())
   , mActionInfo(actionInfo)
   , mActionsService(actionsService)
   {
      labelArea()->doubleClicked().connect(std::bind(&ActionItemView::onDblClicked, this, std::placeholders::_1));
      labelArea()->clicked().connect(std::bind(&ActionItemView::onClick, this, std::placeholders::_1));

      acceptDrops("node");
   }

   virtual void populate() override
   {
      actions::ActionsList result;
      mActionsService.GetChildren(nullptr, &mActionInfo.id(), &result, nullptr);

      for(int i = 0; i < result.list_size(); ++i)
      {
         addChildNode(Wt::cpp14::make_unique<ActionItemView>(result.list(i), mActionsService));
      }
   }

   void reparent(const common::UniqueId& parentId, Wt::WTreeNode* newParentNode)
   {
      mActionInfo.mutable_parentid()->set_guid(parentId.guid());

      common::OperationResultMessage dummy;
      mActionsService.EditElement(nullptr, &mActionInfo, &dummy, nullptr);
      
      newParentNode->addChildNode(parentNode()->removeChildNode(this));
   }

   void dropEvent	(Wt::WDropEvent dropEvent)	
   {
      ActionItemView* node = static_cast<ActionItemView*>(dropEvent.source());
      node->reparent(mActionInfo.id(), this);
   }

   void setMoveMode(const bool moveMode)
   {
      if(moveMode)
      {
         labelArea()->setDraggable("node", nullptr, false, this);
      }
      else
      {
         labelArea()->unsetDraggable();
      }
      
      if(populated())
      {
         for(auto x : childNodes())
         {
            ActionItemView* node = static_cast<ActionItemView*>(x);
            node->setMoveMode(moveMode);
         }
      }
      
   }

private:
   void onDblClicked(Wt::WMouseEvent ev)
   {
      auto dlg = new ActionItemViewEditDialog(
         mActionInfo.title(),
         mActionInfo.description(),
         std::bind(&ActionItemView::onDialogOk, this, std::placeholders::_1, std::placeholders::_2));
      dlg->show();
   }

   void onDialogOk(const std::string& title, const std::string& desc)
   {
      label()->setText(title);

      mActionInfo.set_title(title);
      mActionInfo.set_description(desc);

      common::OperationResultMessage dummy;
      mActionsService.EditElement(nullptr, &mActionInfo, &dummy, nullptr);
   }

   void onClick(Wt::WMouseEvent ev)
   {
      if(ev.modifiers().test(Wt::KeyboardModifier::Control))
      {
          std::function<void()> elementDeletedFunc = [=] () {
            common::OperationResultMessage dummy;
            mActionsService.DeleteElement(nullptr, &mActionInfo.id(), &dummy, nullptr);

            parentNode()->removeChildNode(this);
            };

          CommonDialogManager::showConfirmationDialog("Delete it?", elementDeletedFunc);
      }
      else if(ev.modifiers().test(Wt::KeyboardModifier::Shift))
      {
          auto dlg = new ActionItemViewEditDialog(
          "",
          "",
          [=](const std::string& title, const std::string& desc)
          {
              actions::ActionInfo newItem;
              newItem.set_title(title);
              newItem.set_description(desc);
              newItem.mutable_parentid()->set_guid(mActionInfo.id().guid());

              common::UniqueId* newId = new common::UniqueId;
              mActionsService.AddElement(nullptr, &newItem, newId, nullptr);

              newItem.set_allocated_id(newId);
              
              addChildNode(Wt::cpp14::make_unique<ActionItemView>(newItem, mActionsService));
          });
          dlg->show();
      }
   }

   actions::ActionInfo mActionInfo;
   actions::ActionsService_Stub& mActionsService;
};

class ActionsRootTreeNode : public Wt::WTreeNode
{
public:
   ActionsRootTreeNode(actions::ActionsService_Stub& actionsService)
   : Wt::WTreeNode("Actions")
   , mActionsService(actionsService)
   {
      labelArea()->clicked().connect(std::bind(&ActionsRootTreeNode::onClick, this, std::placeholders::_1));
      acceptDrops("node");
   }

   virtual void populate() override
   {
      common::EmptyMessage dummy;
      actions::ActionsList result;
      mActionsService.GetParentlessElements(nullptr, &dummy, &result, nullptr);

      for(int i = 0; i < result.list_size(); ++i)
      {
         auto node = addChildNode(Wt::cpp14::make_unique<ActionItemView>(result.list(i), mActionsService));

         if(result.list(i).title() == "Backlog")
         {
            mBackLogNode = node;
            mBackLogNode->hide();
         }
      }
   }

   virtual void dropEvent(Wt::WDropEvent dropEvent) override
   {
      ActionItemView* node = dynamic_cast<ActionItemView*>(dropEvent.source());
      if(node == nullptr)
      {
         std::cout << "\nshit\n";
      }
      else
      {
         std::cout << "\nnodeok\n";
      }
      node->reparent(common::UniqueId(), this);
   }

   void setMoveMode(const bool moveMode)
   {
      for(auto x : childNodes())
      {
         ActionItemView* node = static_cast<ActionItemView*>(x);
         node->setMoveMode(moveMode);
      }
   }

   void setShowBacklogMode(const bool isShowBacklog)
   {
      if(mBackLogNode != nullptr)
      {
         if(isShowBacklog)
         {
            mBackLogNode->show();
         }
         else
         {
            mBackLogNode->hide();
         }
      }
   }

private:
   void onClick(Wt::WMouseEvent ev)
   {
      if(ev.modifiers().test(Wt::KeyboardModifier::Shift))
      {
         auto dlg = new ActionItemViewEditDialog(
         "",
         "",
         [=](const std::string& title, const std::string& desc)
         {
            actions::ActionInfo newItem;
            newItem.set_title(title);
            newItem.set_description(desc);

            common::UniqueId* newId = new common::UniqueId;
            mActionsService.AddElement(nullptr, &newItem, newId, nullptr);

            newItem.set_allocated_id(newId);
            addChildNode(Wt::cpp14::make_unique<ActionItemView>(newItem, mActionsService));
         });
         dlg->show();
      }
   }

   Wt::WTreeNode* mBackLogNode = nullptr;
   actions::ActionsService_Stub& mActionsService;
};

ActionsView::ActionsView()
{
   addStyleClass("container-fluid");

   mService.reset(new MateriaServiceProxy<actions::ActionsService>("WebApp"));
   mActions = &mService->getService();

   auto actionsGroup = new Wt::WGroupBox;
   actionsGroup->addStyleClass("col-md-10");

   actionsGroup->addWidget(Wt::cpp14::make_unique< Wt::WText>("Ctrl+click = delete, shift+click = add    "));

   auto tree = new Wt::WTree();
   auto root = new ActionsRootTreeNode(*mActions);

   auto cbMoveMode = new Wt::WCheckBox("Move mode");
   cbMoveMode->checked().connect(std::bind([=](){root->setMoveMode(true);}));
   cbMoveMode->unChecked().connect(std::bind([=](){root->setMoveMode(false);}));
   actionsGroup->addWidget(std::unique_ptr<Wt::WCheckBox>(cbMoveMode));

   auto cbShowBl = new Wt::WCheckBox("Show backlog");
   cbShowBl->checked().connect(std::bind([=](){root->setShowBacklogMode(true);}));
   cbShowBl->unChecked().connect(std::bind([=](){root->setShowBacklogMode(false);}));
   actionsGroup->addWidget(std::unique_ptr<Wt::WCheckBox>(cbShowBl));
   
   tree->setTreeRoot(std::unique_ptr<ActionsRootTreeNode>(root));
   tree->setStyleClass("custom-tree");
   root->setLoadPolicy(Wt::ContentLoading::Lazy);
   root->expand();

   actionsGroup->addWidget(std::unique_ptr<Wt::WTree>(tree));

   addWidget(std::unique_ptr<Wt::WGroupBox>(actionsGroup));
}