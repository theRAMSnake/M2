#include "DatabaseView.hpp"
#include "CommonDialogManager.hpp"
#include <Common/ZmqPbChannel.hpp>
#include <Common/PortLayout.hpp>
#include <Wt/WPushButton.h>
#include <Wt/WGroupBox.h>
#include <Wt/WLabel.h>
#include <Wt/WCssDecorationStyle.h>
#include <Wt/WTable.h>
#include <boost/algorithm/string.hpp>

DatabaseView::DatabaseView()
{
   mService.reset(new MateriaServiceProxy<database::DatabaseService>("WebApp"));
   mDb = &mService->getService();

   Wt::WPushButton* addButton = new Wt::WPushButton("Add");
   addButton->clicked().connect(std::bind(&DatabaseView::onAddClick, this));
   addButton->addStyleClass("btn-primary");
   addWidget(std::unique_ptr<Wt::WPushButton>(addButton));

   Wt::WPushButton* deleteButton = new Wt::WPushButton("Delete");
   deleteButton->clicked().connect(std::bind(&DatabaseView::onDeleteClick, this));
   deleteButton->addStyleClass("btn-primary");
   deleteButton->setMargin(5, Wt::Side::Left);
   addWidget(std::unique_ptr<Wt::WPushButton>(deleteButton));

   Wt::WPushButton* saveButton = new Wt::WPushButton("Save");
   saveButton->clicked().connect(std::bind(&DatabaseView::onSaveClick, this));
   saveButton->addStyleClass("btn-primary");
   saveButton->setMargin(5, Wt::Side::Left);
   addWidget(std::unique_ptr<Wt::WPushButton>(saveButton));

   addWidget(std::make_unique<Wt::WText>("<br></br>"));

   Wt::WLabel* labelCategory = new Wt::WLabel("Category: ");
   labelCategory->setMargin(5, Wt::Side::Left);
   addWidget(std::unique_ptr<Wt::WLabel>(labelCategory));
   mEditCategory = new Wt::WLineEdit("");
   addWidget(std::unique_ptr<Wt::WLineEdit>(mEditCategory));
   labelCategory->setBuddy(mEditCategory);
   //mEditCategory->setWidth(500);

   addWidget(std::make_unique<Wt::WText>("<br></br>"));

   Wt::WLabel* labelQuery = new Wt::WLabel("Key or query: ");
   labelQuery->setMargin(5, Wt::Side::Left);
   addWidget(std::unique_ptr<Wt::WLabel>(labelQuery));
   mEditQuery = new Wt::WLineEdit("");
   addWidget(std::unique_ptr<Wt::WLineEdit>(mEditQuery));
   labelQuery->setBuddy(mEditQuery);

   mEditQuery->enterPressed().connect(std::bind(&DatabaseView::onQueryEditEnterPressed, this));

   addStyleClass("container-fluid");

   auto gr = addWidget(std::make_unique<Wt::WGroupBox>());

   {
      auto subgr = gr->addWidget(std::make_unique<Wt::WGroupBox>());
      subgr->addStyleClass("col-md-4");
      mFindResult = new Wt::WTable();
      mFindResult->addStyleClass("table-bordered");
      mFindResult->addStyleClass("table-hover");
      mFindResult->addStyleClass("table-striped");
      mFindResult->decorationStyle().font().setSize(Wt::WFont::Size::XXLarge);
      subgr->addWidget(std::unique_ptr<Wt::WTable>(mFindResult));
   }
   {
      auto subgr = gr->addWidget(std::make_unique<Wt::WGroupBox>());
      subgr->addStyleClass("col-md-8");
      mDocumentEdit = subgr->addWidget(std::make_unique<Wt::WTextArea>(""));
      mDocumentEdit->setHeight(650);
   }
}

std::string getDefaultJson()
{
   return "{\n   \"content\" : \" \" \n}";
}

void DatabaseView::onAddClick()
{
   database::Document doc;
   doc.set_body(getDefaultJson());
   addItem(doc);
}

void DatabaseView::addItem(database::Document& doc)
{
   auto row = mFindResult->insertRow(mFindResult->rowCount());
   std::string key = doc.header().key();
   row->elementAt(0)->addWidget(std::make_unique<Wt::WText>(key.empty() ? "New" : key));
   row->elementAt(0)->clicked().connect(std::bind(&DatabaseView::onClick, this, row));

   mSelectedRow = row;
   
   mDocumentEdit->setText(doc.body());

   mRowToDocumentMap.insert(std::make_pair(row, doc));
}

void DatabaseView::onDeleteClick()
{
   if(mSelectedRow == nullptr)
   {
      return;
   }

   std::function<void()> elementDeletedFunc = [=] () {
      
      database::Document& doc = mRowToDocumentMap[mSelectedRow];
      if(!doc.header().key().empty())
      {
         common::OperationResultMessage result;
         mDb->DeleteDocument(nullptr, &doc.header(), &result, nullptr);
   
         if(!result.success())
         {
            CommonDialogManager::showMessage("There was an error deleting element");
         }
      }
   
      mRowToDocumentMap.erase(mRowToDocumentMap.find(mSelectedRow));
      mFindResult->removeRow(mSelectedRow->rowNum());
      mSelectedRow = nullptr;
      mDocumentEdit->setText("");

      };

   CommonDialogManager::showConfirmationDialog("Delete it?", elementDeletedFunc);
}

void DatabaseView::onSaveClick()
{
   if(mSelectedRow == nullptr)
   {
      return;
   }
   
   database::Document& doc = mRowToDocumentMap[mSelectedRow];
   if(doc.header().key().empty())
   {
      doc.mutable_header()->set_category(mEditCategory->text().narrow());
      common::UniqueId newId;
      mDb->AddDocument(nullptr, &doc, &newId, nullptr);

      if(!newId.guid().empty())
      {
         doc.mutable_header()->set_key(newId.guid());
         mSelectedRow->elementAt(0)->clear();
         mSelectedRow->elementAt(0)->addWidget(std::make_unique<Wt::WText>(newId.guid()));
      }
      else
      {
         CommonDialogManager::showMessage("There was an error adding element");
      }
   }
   else
   {
      common::OperationResultMessage result;
      doc.set_body(mDocumentEdit->text().narrow());
      mDb->ModifyDocument(nullptr, &doc, &result, nullptr);

      if(!result.success())
      {
         CommonDialogManager::showMessage("There was an error deleting element");
      }
   }
}

void DatabaseView::onClick(Wt::WTableRow* row)
{
   mRowToDocumentMap[mSelectedRow].set_body(mDocumentEdit->text().narrow());

   mSelectedRow = row;
   mDocumentEdit->setText(mRowToDocumentMap[row].body());
}

void DatabaseView::onQueryEditEnterPressed()
{
   clearResult();

   database::Documents result;

   auto query = mEditQuery->text().narrow();
   if(query.find('=') != std::string::npos)
   {
      std::vector<std::string> subQueries;
      boost::split(subQueries, query, boost::is_any_of(";"));

      database::DocumentQuery docQuery;
      docQuery.set_category(mEditCategory->text().narrow());
      for(auto x : subQueries)
      {
         std::string s = x;
         boost::algorithm::trim(s);
         auto pos = s.find('=');
         if(pos != std::string::npos)
         {
            auto kval = docQuery.add_query();
            kval->set_key(s.substr(0, pos));
            kval->set_value(s.substr(pos + 1));
         }

         mDb->SearchDocuments(nullptr, &docQuery, &result, nullptr);
      }
   }
   else
   {
      database::DocumentHeader header;
      header.set_category(mEditCategory->text().narrow());
      header.set_key(mEditQuery->text().narrow());

      mDb->GetDocument(nullptr, &header, &result, nullptr);
      
   }

   for(auto d : result.result())
   {
      addItem(d);
   }
}

void DatabaseView::clearResult()
{
   std::vector<std::map<Wt::WTableRow*, database::Document>::const_iterator> rowsToDelete;
   for(auto x = mRowToDocumentMap.begin(); x != mRowToDocumentMap.end(); ++x)
   {
      if(!x->second.header().key().empty())
      {
         rowsToDelete.push_back(x);
      }
   }

   for(auto x : rowsToDelete)
   {
      mFindResult->removeRow(x->first->rowNum());
      if(mSelectedRow == x->first)
      {
         mSelectedRow = nullptr;
         mDocumentEdit->setText("");
      }

      mRowToDocumentMap.erase(x);
   }
}