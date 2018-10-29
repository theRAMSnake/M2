#include "JournalView.hpp"
#include "CommonDialogManager.hpp"
#include <Wt/WGroupBox.h>
#include <Wt/WTree.h>
#include <Wt/WTreeNode.h>
#include <Wt/WText.h>

class JournalTreeNode : public Wt::WTreeNode
{
public:
   JournalTreeNode(const IndexItem& item, JournalModel& model)
   : Wt::WTreeNode("Journal")
   , mItem(item)
   , mModel(model)
   {
      labelArea()->doubleClicked().connect(std::bind(&JournalTreeNode::onDblClicked, this, std::placeholders::_1));
      labelArea()->clicked().connect(std::bind(&JournalTreeNode::onClick, this, std::placeholders::_1));
   }

   void populate() override
   {
      for(auto x : mModel.getChildren(mItem.id))
      {
         addChildNode(std::make_unique<JournalTreeNode>(x, mModel));
      }
   }

private:
    void onDblClicked(Wt::WMouseEvent ev)
   {
      /*auto dlg = new ActionsTreeNodeEditDialog(
         mActionInfo.title(),
         mActionInfo.description(),
         std::bind(&ActionsTreeNode::onDialogOk, this, std::placeholders::_1, std::placeholders::_2));
      dlg->show();*/
   }

   void onClick(Wt::WMouseEvent ev)
   {
      if(ev.modifiers().test(Wt::KeyboardModifier::Control))
      {
         /* std::function<void()> elementDeletedFunc = [=] () {
            common::OperationResultMessage dummy;
            mActionsService.DeleteElement(nullptr, &mActionInfo.id(), &dummy, nullptr);
             parentNode()->removeChildNode(this);
            delete this;
            };
           CommonDialogManager::showConfirmationDialog("Delete it?", elementDeletedFunc);*/
      }
      else if(ev.modifiers().test(Wt::KeyboardModifier::Shift) && !mItem.isPage)
      {
         std::vector<std::string> choise = {"Page", "Folder"};
         CommonDialogManager::showChoiseDialog(choise, [=](auto selected) {
            const bool isPage = selected == 0;

            std::function<void(std::string)> nextFunc = [this, isPage](std::string title){
               auto item = mModel.addIndexItem(isPage, title, mItem.id);
               addChildNode(std::make_unique<JournalTreeNode>(item, mModel));
            };

            CommonDialogManager::showOneLineDialog("Please specify title", "Title", "", nextFunc);
         });
      }
   }

   IndexItem mItem;
   JournalModel& mModel;
};

//-------------------------------------------------------------------------------------------------------

JournalView::JournalView(JournalModel& model)
: mModel(model)
{
   auto indexGroup = new Wt::WGroupBox;
   indexGroup->addStyleClass("col-md-2");
   indexGroup->addWidget(std::unique_ptr<Wt::WWidget>(createIndexView()));

   auto mainGroup = new Wt::WGroupBox;
   mainGroup->addStyleClass("col-md-10");
   mainGroup->addWidget(std::unique_ptr<Wt::WWidget>(createPageView()));
   
   addWidget(std::unique_ptr<Wt::WGroupBox>(indexGroup));
   addWidget(std::unique_ptr<Wt::WGroupBox>(mainGroup));
}

Wt::WWidget* JournalView::createIndexView()
{
   auto result = new Wt::WContainerWidget();

   std::unique_ptr<Wt::WTree> tree = std::make_unique<Wt::WTree>();
   IndexItem empty;
   auto root = std::make_unique<JournalTreeNode>(empty, mModel);
   tree->setTreeRoot(std::move(root));
   tree->setSelectionMode(Wt::SelectionMode::Single);

   tree->treeRoot()->label()->setTextFormat(Wt::TextFormat::Plain);
   tree->treeRoot()->setLoadPolicy(Wt::ContentLoading::NextLevel);
   tree->treeRoot()->expand();

   result->addWidget(std::move(tree));

   return result;
}

Wt::WWidget* JournalView::createPageView()
{
   return new Wt::WContainerWidget();
}