#include "JournalView.hpp"
#include "dialog/CommonDialogManager.hpp"
#include "dialog/Dialog.hpp"
#include <Wt/WGroupBox.h>
#include <Wt/WTree.h>
#include <Wt/WTreeNode.h>
#include <Wt/WText.h>
#include <Wt/WTable.h>
#include <Wt/WTextEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WLineEdit.h>
#include <Wt/WCssDecorationStyle.h>
#include <Wt/WCheckBox.h>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

std::string fetchLine(const std::string& content, const std::size_t pos)
{
   auto start = content.rfind('\n', pos);
   auto end = content.find('\n', pos);

   if(start == std::string::npos)
   {
      start = 0;
   }

   if(end == std::string::npos)
   {
      return content.substr(start);
   }
   else
   {
      return content.substr(start, end - start);
   }
}

void eraseAllSubStr(std::string & mainStr, const std::string & toErase)
{
	size_t pos = std::string::npos;
 
	// Search for the substring in string in a loop untill nothing is found
	while ((pos  = mainStr.find(toErase) )!= std::string::npos)
	{
		// If found then erase it from string
		mainStr.erase(pos, toErase.length());
	}
}

std::string buildSample(
   JournalModel& model,
   const materia::Id& pageId, 
   const std::size_t pos, 
   const std::string& keyword
   )
{

   auto content = model.loadContent(pageId);
   boost::algorithm::to_lower(content);

   auto kw = keyword;
   boost::algorithm::to_lower(kw);

   auto line = fetchLine(content, pos);
   auto posInLine = line.find(kw);

   line.insert(posInLine + kw.size(), "</b>");
   line.insert(posInLine, "<b>");

   eraseAllSubStr(line, "<li>");
   eraseAllSubStr(line, "</li>");
   
   return line;
}

class JournalSearchView : public Wt::WContainerWidget
{
public:
   JournalSearchView(JournalModel& model, std::function<void(const materia::Id, std::size_t)> navigateFunction)
   {
      auto tv = new Wt::WLineEdit();
      addWidget(std::unique_ptr<Wt::WWidget>(tv));

      auto gb = new Wt::WGroupBox();
      addWidget(std::unique_ptr<Wt::WGroupBox>(gb));
      
      auto table = new Wt::WTable();
      table->setWidth(Wt::WLength("100%"));
      table->addStyleClass("table-bordered");
      table->addStyleClass("table-hover");
      table->addStyleClass("table-striped");
      table->decorationStyle().font().setSize(Wt::WFont::Size::Large);
      gb->addWidget(std::unique_ptr<Wt::WTable>(table));

      tv->textInput().connect(std::bind( [table, tv, &model, navigateFunction]() 
      {
         table->clear();
         auto text = tv->text().narrow();
         if(text.size() > 2)
         {
            auto result = model.searchContent(text);

            const std::size_t SEARCH_RESULT_LIMIT = 25;
            for(std::size_t i = 0; i < result.size() && i < SEARCH_RESULT_LIMIT; ++i)
            {
               auto cell = table->elementAt(i, 0);
               auto pageId = result[i].pageId;
               auto pos = result[i].position;
               auto sample = new Wt::WText(buildSample(model, pageId, pos, text));
               sample->setMargin(Wt::WLength(50));
               sample->clicked().connect(std::bind( [=]() {
                  navigateFunction(pageId, pos);
               }));
               cell->addWidget(std::unique_ptr<Wt::WText>(sample));
            }
         }
      }));
   }
};

//----------------------------------------------------------------------------------------------------------------------------

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
      auto ch = mModel.getChildren(mItem.id);

      for(auto x : ch)
      {
         if(!x.isPage)
         {
            addChildNode(std::make_unique<JournalTreeNode>(x, mModel));
         }
      }

      for(auto x : ch)
      {
         if(x.isPage)
         {
            addChildNode(std::make_unique<JournalTreeNode>(x, mModel));
         }
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
      if(ev.modifiers().test(Wt::KeyboardModifier::Control) && mItem.isPage)
      {
        if(mItem.id != materia::Id::Invalid)
        {
          std::function<void()> elementDeletedFunc = [this] () {
              mModel.deleteItem(mItem.id);
              parentNode()->removeChildNode(this);
            };
          CommonDialogManager::showConfirmationDialog("Delete it?", elementDeletedFunc);
        }
      }
      else if(ev.modifiers().test(Wt::KeyboardModifier::Control) && !mItem.isPage)
      {
         CommonDialogManager::showBinaryChoiseDialog("Clear", "Erase", 
         [this]() 
         {
            std::function<void()> elementClearFunc = [this] () {
               mModel.clearItem(mItem.id);
               for(auto c : childNodes())
               {
                  removeChildNode(c);
               }
            };

            CommonDialogManager::showConfirmationDialog("Clear it?", elementClearFunc);
         },
         [this]() 
         {
            std::function<void()> elementDeletedFunc = [this] () {
               mModel.deleteItem(mItem.id);
               parentNode()->removeChildNode(this);
            };

            CommonDialogManager::showConfirmationDialog("Delete it?", elementDeletedFunc);
         }
         );
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

   auto moveBtn = new Wt::WPushButton("Move");
   moveBtn->addStyleClass("btn-primary");
   moveBtn->clicked().connect(std::bind(&JournalView::onMoveClick, this));
   moveBtn->setMargin(5);
   result->addWidget(std::unique_ptr<Wt::WPushButton>(moveBtn));

   auto searchBtn = new Wt::WPushButton("Search");
   searchBtn->addStyleClass("btn-primary");
   searchBtn->clicked().connect(std::bind(&JournalView::onSearchClick, this));
   searchBtn->setMargin(5);
   result->addWidget(std::unique_ptr<Wt::WPushButton>(searchBtn));

   mSaveBtn = new Wt::WPushButton("Save");
   mSaveBtn->clicked().connect(std::bind(&JournalView::onSaveClick, this));
   mSaveBtn->addStyleClass("btn-primary");
   mSaveBtn->setEnabled(false);
   mSaveBtn->setMargin(5);
   result->addWidget(std::unique_ptr<Wt::WPushButton>(mSaveBtn));

   auto randomBtn = new Wt::WPushButton("Random");
   randomBtn->addStyleClass("btn-primary");
   randomBtn->clicked().connect(std::bind(&JournalView::onRandomClick, this));
   randomBtn->setMargin(5);
   result->addWidget(std::unique_ptr<Wt::WPushButton>(randomBtn));

   mIsSharedCheckbox = new Wt::WCheckBox("Shared");
   mIsSharedCheckbox->checked().connect(std::bind(&JournalView::onSharedPageChecked, this));
   mIsSharedCheckbox->unChecked().connect(std::bind(&JournalView::onSharedPageUnchecked, this));
   result->addWidget(std::unique_ptr<Wt::WCheckBox>(mIsSharedCheckbox));

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

   return result;
}

void JournalView::onSearchClick()
{
   std::function<void(const materia::Id, std::size_t)> f = [=] (const materia::Id id, std::size_t pos) {
         mDlg->stop();
         navigate(id, pos);
      };
   
   mDlg = new Dialog(
      "Journal Search", 
      std::make_unique<JournalSearchView>(mModel, f));

   mDlg->show();
}

void JournalView::onSaveClick()
{
   auto node = static_cast<JournalTreeNode*>(*mIndexTree->selectedNodes().begin());
   auto& item = node->getItem();
   mModel.saveContent(item.id, mPageView->text().narrow());

   if(isPageShared(item.id))
   {
      saveSharedPage(item.title, mPageView->text().narrow());
   }

   mIndexTree->clearSelection();
}

class FolderTreeNode : public Wt::WTreeNode
{
public:
   FolderTreeNode(const IndexItem& item, JournalModel& model)
   : Wt::WTreeNode(item.id == materia::Id::Invalid ? "Journal" : item.title)
   , mItem(item)
   , mModel(model)
   {
      auto iconName = "resources/Folder.gif";
      setLabelIcon(std::make_unique<Wt::WIconPair>(iconName, iconName, false));
   }

   void populate() override
   {
      auto ch = mModel.getChildren(mItem.id);

      for(auto x : ch)
      {
         if(!x.isPage)
         {
            addChildNode(std::make_unique<FolderTreeNode>(x, mModel));
         }
      }
   }

   const IndexItem& getItem()
   {
      return mItem;
   }

private:

   const IndexItem mItem;
   JournalModel& mModel;
};

class FolderSelectionDialog : public BasicDialog
{
public:
   FolderSelectionDialog(JournalModel& model, std::function<void(materia::Id)> cb)
   : BasicDialog("Select folder", true)
   {
      std::unique_ptr<Wt::WTree> tree = std::make_unique<Wt::WTree>();
      IndexItem empty;
      auto root = std::make_unique<FolderTreeNode>(empty, model);
      tree->setTreeRoot(std::move(root));
      tree->setSelectionMode(Wt::SelectionMode::Single);

      tree->treeRoot()->label()->setTextFormat(Wt::TextFormat::Plain);
      tree->treeRoot()->setLoadPolicy(Wt::ContentLoading::NextLevel);
      tree->treeRoot()->expand();

      auto treePtr = tree.get();

      contents()->addWidget(std::move(tree));

      setWidth("50%");
      setHeight("70%");

      finished().connect(std::bind([=]() {
         if(!treePtr->selectedNodes().empty())
         {
            auto node = static_cast<FolderTreeNode*>(*treePtr->selectedNodes().begin());
            cb(node->getItem().id);
         }
         delete this;
      }));
   }
};

void JournalView::onMoveClick()
{
   if(!mIndexTree->selectedNodes().empty())
   {
      auto node = static_cast<JournalTreeNode*>(*mIndexTree->selectedNodes().begin());
      auto& item = node->getItem();

      auto dlg = new FolderSelectionDialog(mModel, [=](materia::Id newFolderId){
         mModel.moveIndexItem(item.id, newFolderId);
         auto freeNode = node->parentNode()->removeChildNode(node);
         findNodeById(mIndexTree->treeRoot(), newFolderId)->addChildNode(std::move(freeNode));
      });
      dlg->show();
   }
}

void JournalView::saveSharedPage(const std::string& title, const std::string& content)
{
   std::ofstream file("/materia/shared/" + title, std::ios_base::trunc | std::ios_base::out);
   file << content;
}

void JournalView::onSharedPageChecked()
{
   auto node = static_cast<JournalTreeNode*>(*mIndexTree->selectedNodes().begin());
   auto& item = node->getItem();

   saveSharedPage(item.title, mPageView->text().narrow());
}

void JournalView::eraseSharedPage(const std::string& title) const
{
   boost::filesystem::remove("/materia/shared/" + title);
}

void JournalView::onSharedPageUnchecked()
{
   auto node = static_cast<JournalTreeNode*>(*mIndexTree->selectedNodes().begin());
   auto& item = node->getItem();

   eraseSharedPage(item.title);
}

bool JournalView::isPageShared(const std::string& title)
{
   return boost::filesystem::exists("/materia/shared/" + title);
}

void JournalView::onRandomClick()
{
   auto text = mPageView->text().narrow();

   std::vector<string> strs;
   boost::split(strs, text, boost::is_any_of("\n\r"), boost::token_compress_on);

   srand (time(NULL));

   auto result = strs[rand() % strs.size()];
   CommonDialogManager::showMessage(result);
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
         mIsSharedCheckbox->setEnabled(true);
         mPageView->setText(mModel.loadContent(item.id));
         mIsSharedCheckbox->setChecked(isPageShared(item.title));
      }
      else
      {
         mSaveBtn->setEnabled(false);
         mPageView->setText("");
         mIsSharedCheckbox->setEnabled(false);
      }
   }
   else
   {
      mSaveBtn->setEnabled(false);
      mIsSharedCheckbox->setEnabled(false);
      mPageView->setText("");
   }
}

void JournalView::navigate(const materia::Id id, std::size_t pos)
{
   auto node = findNodeById(mIndexTree->treeRoot(), id);
   mIndexTree->select(node);

   while(node->parentNode() != nullptr)
   {
      node->parentNode()->expand();
      node = node->parentNode();
   }
}

Wt::WTreeNode* JournalView::findNodeById(Wt::WTreeNode* node, const materia::Id id)
{
   auto nodeCasted = static_cast<JournalTreeNode*>(node);
   if(nodeCasted->getItem().id == id)
   {
      return node;
   }

   for(auto x : node->childNodes())
   {
      auto ret = findNodeById(x, id);
      if(ret != nullptr)
      {
         return ret;
      }
   }

   return nullptr;
}