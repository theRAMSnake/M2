#include "JournalView.hpp"
#include "CommonDialogManager.hpp"
#include <Wt/WGroupBox.h>
#include <Wt/WTree.h>
#include <Wt/WTreeNode.h>
#include <Wt/WText.h>
#include <Wt/WTextEdit.h>
#include <Wt/WPushButton.h>

class JournalTreeNode : public Wt::WTreeNode
{
public:
   JournalTreeNode(const IndexItem& item, JournalModel& model)
   : Wt::WTreeNode(item.id == materia::Id::Invalid ? "Journal" : item.title)
   , mItem(item)
   , mModel(model)
   {
      labelArea()->doubleClicked().connect(std::bind(&JournalTreeNode::onDblClicked, this, std::placeholders::_1));
      labelArea()->clicked().connect(std::bind(&JournalTreeNode::onClick, this, std::placeholders::_1));

      if(!item.isPage)
      {
        auto iconName = "resources/Folder.gif";
        setLabelIcon(std::make_unique<Wt::WIconPair>(iconName, iconName, false));
      }
   }

   void populate() override
   {
      for(auto x : mModel.getChildren(mItem.id))
      {
         addChildNode(std::make_unique<JournalTreeNode>(x, mModel));
      }
   }

   const IndexItem& getItem()
   {
      return mItem;
   }

private:
   void onDblClicked(Wt::WMouseEvent ev)
   {
     if(mItem.id != materia::Id::Invalid)
     {
       std::function<void(std::string)> nextFunc = [this](std::string newTitle){
               mModel.renameIndexItem(mItem.id, newTitle);
               label()->setText(newTitle);
            };

       CommonDialogManager::showOneLineDialog("Please specify title", "Title", mItem.title, nextFunc);
     }
   }

   void onClick(Wt::WMouseEvent ev)
   {
      if(ev.modifiers().test(Wt::KeyboardModifier::Control))
      {
        if(mItem.id != materia::Id::Invalid)
        {
          std::function<void()> elementDeletedFunc = [=] () {
              mModel.deleteItem(mItem.id);
              parentNode()->removeChildNode(this);
            };
          CommonDialogManager::showConfirmationDialog("Delete it?", elementDeletedFunc);
        }
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
   tree->itemSelectionChanged().connect(this, &JournalView::onIndexSelectionChanged);

   mIndexTree = tree.get();

   result->addWidget(std::move(tree));

   return result;
}

Wt::WWidget* JournalView::createPageView()
{
   auto result = new Wt::WContainerWidget();

   mPageView = new Wt::WTextEdit();
   mPageView->setConfigurationSetting("branding", false);
   mPageView->setConfigurationSetting("elementpath", false);
   mPageView->setConfigurationSetting("Browser_spellcheck", true);
   mPageView->setConfigurationSetting("statusbar", false);
   mPageView->setConfigurationSetting("menubar", "edit format table");
   
   mPageView->setExtraPlugins("colorpicker, textcolor, searchreplace, table, lists");
   mPageView->setToolBar(0, "bold italic | link | forecolor backcolor | fontsizeselect | numlist bullist");
   mPageView->setHeight(800);
   
   result->addWidget(std::unique_ptr<Wt::WTextEdit>(mPageView));

   mSaveBtn = new Wt::WPushButton("Save");
   mSaveBtn->clicked().connect(std::bind(&JournalView::onSaveClick, this));
   mSaveBtn->addStyleClass("btn-primary");
   mSaveBtn->setMargin(5);
   mSaveBtn->setEnabled(false);
   result->addWidget(std::unique_ptr<Wt::WPushButton>(mSaveBtn));

   return result;
}

void JournalView::onSaveClick()
{
   auto node = static_cast<JournalTreeNode*>(*mIndexTree->selectedNodes().begin());
   auto& item = node->getItem();
   mModel.saveContent(item.id, mPageView->text().narrow());
}

void JournalView::onIndexSelectionChanged()
{
   auto nodes = mIndexTree->selectedNodes();
   if(nodes.size())
   {
      auto node = static_cast<JournalTreeNode*>(*nodes.begin());
      auto& item = node->getItem();
      if(item.isPage)
      {
         mSaveBtn->setEnabled(true);
         mPageView->setText(mModel.loadContent(item.id));
      }
      else
      {
         mSaveBtn->setEnabled(false);
         mPageView->setText("");
      }
   }
}