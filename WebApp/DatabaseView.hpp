#pragma once
#include <Wt/WContainerWidget.h>
#include <Wt/WEvent.h>
#include <Wt/WLineEdit.h>
#include <Wt/WTextArea.h>
#include <Wt/WTable.h>
#include <messages/database.pb.h>
#include "Common/MateriaServiceProxy.hpp"

class DatabaseView : public Wt::WContainerWidget
{
public:
   DatabaseView();

private:
   void onAddClick();
   void onDeleteClick();
   void onSaveClick();
   void onClick(Wt::WTableRow* row);
   void onQueryEditEnterPressed();

   void addItem(database::Document& doc);
   void clearResult();
   
   std::unique_ptr<MateriaServiceProxy<database::DatabaseService>> mService;
   database::DatabaseService_Stub* mDb;

   Wt::WLineEdit* mEditCategory;
   Wt::WTable* mFindResult;
   Wt::WTextArea* mDocumentEdit;
   Wt::WLineEdit* mEditQuery;

   std::map<Wt::WTableRow*, database::Document> mRowToDocumentMap;
   Wt::WTableRow* mSelectedRow = nullptr;
};