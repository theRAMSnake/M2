#include "InboxView.hpp"
#include "CommonDialogManager.hpp"
#include <Common/ZmqPbChannel.hpp>
#include <Common/PortLayout.hpp>
#include <Wt/WText>
#include <Wt/WCssDecorationStyle>
#include <Wt/WPushButton>
#include <Wt/WGroupBox>

InboxView::InboxView()
{
   Wt::WPushButton* addButton = new Wt::WPushButton("Add");
   addButton->clicked().connect(std::bind(&InboxView::onAddClick, this));
   addButton->addStyleClass("btn-primary");
   addWidget(addButton);
   Wt::WGroupBox* gb = new Wt::WGroupBox();
   addWidget(gb);
   
   mTable = new Wt::WTable();
   mTable->setWidth(Wt::WLength("100%"));
   mTable->addStyleClass("table-bordered");
   mTable->addStyleClass("table-hover");
   mTable->addStyleClass("table-striped");
   mTable->decorationStyle().font().setSize(Wt::WFont::Size::XXLarge);
   gb->addWidget(mTable);

   mService.reset(new MateriaServiceProvider<inbox::InboxService>());
   mInbox = &mService->getService();
   
   common::EmptyMessage msg;
   mInbox->GetInbox(nullptr, &msg, &mItems, nullptr);

   for(int i = 0; i < mItems.items_size(); ++i)
   {
      createCellAtRow(i);
   }
}

void InboxView::onItemDoubleClick(Wt::WTableCell* cell, const std::string& itemId)
{
   std::function<void(std::string)> elementModifiedFunc = [=] (std::string a) {
      static_cast<Wt::WText*>(cell->widget(0))->setText(a);
      commitItemEdit(itemId, a);
   };

   CommonDialogManager::showOneLineDialog("Edit", "Text", static_cast<Wt::WText*>(cell->widget(0))->text().narrow(), elementModifiedFunc);
}

void InboxView::onClick(Wt::WMouseEvent ev, Wt::WTableCell* cell, const std::string& itemId)
{
   if(ev.modifiers() & Wt::KeyboardModifier::ControlModifier)
   {
      std::function<void()> elementDeletedFunc = [=] () {
         mTable->deleteRow(cell->row());
         commitItemDelete(itemId);
         };

      CommonDialogManager::showConfirmationDialog("Delete it?", elementDeletedFunc);
   }
   
}

void InboxView::commitItemEdit(const std::string& itemId, const std::string& newText)
{
   for(int i = 0; i < mItems.items_size(); ++i)
   {
      if(mItems.items(i).id().guid() == itemId)
      {
         mItems.mutable_items(i)->set_text(newText);

         common::OperationResultMessage dummy;
         mInbox->EditItem(nullptr, &mItems.items(i), &dummy, nullptr);
         break;
      }
   }
}

void InboxView::commitItemDelete(const std::string& itemId)
{
   for(int i = 0; i < mItems.items_size(); ++i)
   {
      if(mItems.items(i).id().guid() == itemId)
      {
         common::OperationResultMessage dummy;
         mInbox->DeleteItem(nullptr, &mItems.items(i).id(), &dummy, nullptr);
         break;
      }
   }
}

void InboxView::commitItemAdd(inbox::InboxItemInfo& item)
{
   common::UniqueId id;
   mInbox->AddItem(nullptr, &item, &id, nullptr);

   item.mutable_id()->set_guid(id.guid());
}

void InboxView::onAddClick()
{
   std::function<void(std::string)> elementAddedFunc = [=] (std::string a) {
      auto item = mItems.add_items();
      item->set_text(a);

      commitItemAdd(*item);
      createCellAtRow(mTable->rowCount());
   };

   CommonDialogManager::showOneLineDialog("Add", "Text", "", elementAddedFunc);
}

void InboxView::createCellAtRow(const int row)
{
   auto cell = mTable->elementAt(row, 0);
   auto text = new Wt::WText(mItems.items(row).text());
   text->setMargin(Wt::WLength(50));
   cell->addWidget(text);

   cell->clicked().connect(std::bind(&InboxView::onClick, this, std::placeholders::_1, cell, mItems.items(row).id().guid()));
   cell->doubleClicked().connect(std::bind(&InboxView::onItemDoubleClick, this, cell, mItems.items(row).id().guid()));
}