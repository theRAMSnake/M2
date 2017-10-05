#include "ActionsView.hpp"
#include "CommonDialogManager.hpp"
#include <Common/ZmqPbChannel.hpp>
#include <Common/PortLayout.hpp>
#include <Wt/WTree.h>
#include <Wt/WTreeNode.h>
#include <Wt/WCssDecorationStyle.h>
#include <Wt/WTextEdit.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WDialog.h>
#include <Wt/WCheckBox.h>

class ActionsTreeNodeEditDialog : public Wt::WDialog
{
public:
   typedef std::function<void(const std::string&, const std::string&)> TOnOkCallback;
   ActionsTreeNodeEditDialog(const std::string& title, const std::string& desc, TOnOkCallback cb)
   {
      setWidth(Wt::WLength("75%"));
      mTitle = new Wt::WLineEdit(title);
      contents()->addWidget(std::unique_ptr<Wt::WLineEdit>(mTitle));

      mDesc = new Wt::WTextEdit();
      mDesc->setHeight(500);
      mDesc->setConfigurationSetting("branding", false);
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

class ActionsTreeNode : public Wt::WTreeNode
{
public:
   ActionsTreeNode(const actions::ActionInfo& actionInfo, actions::ActionsService_Stub& actionsService)
   : Wt::WTreeNode(actionInfo.title())
   , mActionInfo(actionInfo)
   , mActionsService(actionsService)
   {
      labelArea()->doubleClicked().connect(std::bind(&ActionsTreeNode::onDblClicked, this, std::placeholders::_1));
      labelArea()->clicked().connect(std::bind(&ActionsTreeNode::onClick, this, std::placeholders::_1));

      acceptDrops("node");
   }

   virtual void populate() override
   {
      actions::ActionsList result;
      mActionsService.GetChildren(nullptr, &mActionInfo.id(), &result, nullptr);

      for(int i = 0; i < result.list_size(); ++i)
      {
         addChildNode(Wt::cpp14::make_unique<ActionsTreeNode>(result.list(i), mActionsService));
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
      ActionsTreeNode* node = static_cast<ActionsTreeNode*>(dropEvent.source());
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
            ActionsTreeNode* node = static_cast<ActionsTreeNode*>(x);
            node->setMoveMode(moveMode);
         }
      }
      
   }

private:
   void onDblClicked(Wt::WMouseEvent ev)
   {
      auto dlg = new ActionsTreeNodeEditDialog(
         mActionInfo.title(),
         mActionInfo.description(),
         std::bind(&ActionsTreeNode::onDialogOk, this, std::placeholders::_1, std::placeholders::_2));
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
            delete this;
            };

          CommonDialogManager::showConfirmationDialog("Delete it?", elementDeletedFunc);
      }
      else if(ev.modifiers().test(Wt::KeyboardModifier::Shift))
      {
          auto dlg = new ActionsTreeNodeEditDialog(
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
              
              addChildNode(Wt::cpp14::make_unique<ActionsTreeNode>(newItem, mActionsService));
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
         addChildNode(Wt::cpp14::make_unique<ActionsTreeNode>(result.list(i), mActionsService));
      }
   }

   virtual void dropEvent(Wt::WDropEvent dropEvent) override
   {
      ActionsTreeNode* node = dynamic_cast<ActionsTreeNode*>(dropEvent.source());
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
         ActionsTreeNode* node = static_cast<ActionsTreeNode*>(x);
         node->setMoveMode(moveMode);
      }
   }

private:
   void onClick(Wt::WMouseEvent ev)
   {
      if(ev.modifiers().test(Wt::KeyboardModifier::Shift))
      {
         auto dlg = new ActionsTreeNodeEditDialog(
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
            addChildNode(Wt::cpp14::make_unique<ActionsTreeNode>(newItem, mActionsService));
         });
         dlg->show();
      }
   }

   actions::ActionsService_Stub& mActionsService;
};

ActionsView::ActionsView()
{
   mService.reset(new MateriaServiceProxy<actions::ActionsService>("WebApp"));
   mActions = &mService->getService();

   addWidget(Wt::cpp14::make_unique< Wt::WText>("Ctrl+click = delete, shift+click = add    "));

   auto tree = new Wt::WTree();
   auto root = new ActionsRootTreeNode(*mActions);

   auto cbMoveMode = new Wt::WCheckBox("Move mode");
   cbMoveMode->checked().connect(std::bind([=](){root->setMoveMode(true);}));
   cbMoveMode->unChecked().connect(std::bind([=](){root->setMoveMode(false);}));
   addWidget(std::unique_ptr<Wt::WCheckBox>(cbMoveMode));

   
   tree->setTreeRoot(std::unique_ptr<ActionsRootTreeNode>(root));
   tree->setStyleClass("custom-tree");
   //root->setImagePack("resources/");
   root->setLoadPolicy(Wt::ContentLoading::Lazy);
   root->expand();

   addWidget(std::unique_ptr<Wt::WTree>(tree));
}