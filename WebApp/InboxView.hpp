#pragma once
#include <Wt/WContainerWidget>
#include <Wt/WTable>
#include <Wt/WEvent>
#include <messages/inbox.pb.h>
#include "Common/MateriaServiceProxy.hpp"

class InboxView : public Wt::WContainerWidget
{
public:
   InboxView();

private:
   void onItemDoubleClick(Wt::WTableCell* cell, const std::string& itemId);
   void onClick(Wt::WMouseEvent ev, Wt::WTableCell* cell, const std::string& itemId);
   void onAddClick();
   void createCellAtRow(const int row);
   void commitItemAdd(inbox::InboxItemInfo& item);
   void commitItemEdit(const std::string& itemId, const std::string& newText);
   void commitItemDelete(const std::string& itemId);

   Wt::WTable* mTable;
   std::unique_ptr<MateriaServiceProxy<inbox::InboxService>> mService;
   inbox::InboxService_Stub* mInbox;

   inbox::InboxItems mItems;
};