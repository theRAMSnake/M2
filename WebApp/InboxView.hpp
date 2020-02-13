#pragma once
#include <Wt/WContainerWidget.h>
#include <Wt/WTable.h>
#include <Wt/WEvent.h>
#include "materia/InboxModel.hpp"

class InboxView : public Wt::WContainerWidget
{
public:
   InboxView(InboxModel& model);

private:
   void onItemDoubleClick(Wt::WTableCell* cell, const std::string& itemId);
   void onClick(Wt::WMouseEvent ev, Wt::WTableCell* cell, const std::string& itemId);
   void onAddClick();
   void onClearClick();
   void createCellAtRow(const int row, const InboxModel::Item& item);
   materia::Id commitItemAdd(const std::string& text);
   void commitItemEdit(const std::string& itemId, const std::string& newText);
   void commitItemDelete(const std::string& itemId);

   Wt::WTable* mTable;
   InboxModel& mModel;
};