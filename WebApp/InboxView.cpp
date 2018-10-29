#include "InboxView.hpp"
#include "CommonDialogManager.hpp"
#include <Wt/WText.h>
#include <Wt/WCssDecorationStyle.h>
#include <Wt/WPushButton.h>
#include <Wt/WGroupBox.h>

InboxView::InboxView(InboxModel& model)
: mModel(model)
{
   Wt::WPushButton* addButton = new Wt::WPushButton("Add");
   addButton->clicked().connect(std::bind(&InboxView::onAddClick, this));
   addButton->addStyleClass("btn-primary");
   addWidget(std::unique_ptr<Wt::WPushButton>(addButton));
   auto gb = new Wt::WGroupBox();
   addWidget(std::unique_ptr<Wt::WGroupBox>(gb));
   
   mTable = new Wt::WTable();
   mTable->setWidth(Wt::WLength("100%"));
   mTable->addStyleClass("table-bordered");
   mTable->addStyleClass("table-hover");
   mTable->addStyleClass("table-striped");
   mTable->decorationStyle().font().setSize(Wt::WFont::Size::XXLarge);
   gb->addWidget(std::unique_ptr<Wt::WTable>(mTable));

   auto items = mModel.get();
   for(std::size_t i = 0; i < items.size(); ++i)
   {
      createCellAtRow(i, items[i]);
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
   if(ev.modifiers().test(Wt::KeyboardModifier::Control))
   {
      std::function<void()> elementDeletedFunc = [=] () {
         mTable->removeRow(cell->row());
         commitItemDelete(itemId);
         };

      CommonDialogManager::showConfirmationDialog("Delete it?", elementDeletedFunc);
   }
   
}

void InboxView::commitItemEdit(const std::string& itemId, const std::string& newText)
{
   mModel.replace({itemId, newText});
}

void InboxView::commitItemDelete(const std::string& itemId)
{
   mModel.erase(itemId);
}

materia::Id InboxView::commitItemAdd(const std::string& text)
{
   return mModel.add({materia::Id::Invalid, text});
}

void InboxView::onAddClick()
{
   std::function<void(std::string)> elementAddedFunc = [=] (std::string a) {
      auto id = commitItemAdd(a);
      createCellAtRow(mTable->rowCount(), {id, a});
   };

   CommonDialogManager::showOneLineDialog("Add", "Text", "", elementAddedFunc);
}

void InboxView::createCellAtRow(const int row, const InboxModel::Item& item)
{
   auto cell = mTable->elementAt(row, 0);
   auto text = new Wt::WText(item.text);
   text->setMargin(Wt::WLength(50));
   cell->addWidget(std::unique_ptr<Wt::WText>(text));

   cell->clicked().connect(std::bind(&InboxView::onClick, this, std::placeholders::_1, cell, item.id));
   cell->doubleClicked().connect(std::bind(&InboxView::onItemDoubleClick, this, cell, item.id));
}