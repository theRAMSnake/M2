#include "FinanceView.hpp"
#include <Wt/WGroupBox.h>
#include <Wt/WPushButton.h>
#include <Wt/WDialog.h>
#include <Wt/WLabel.h>
#include <Wt/WTable.h>
#include <Wt/WCssDecorationStyle.h>
#include <Wt/WDateEdit.h>
#include "materia/FinanceModel.hpp"
#include "dialog/Dialog.hpp"
#include "dialog/CommonDialogManager.hpp"
#include <boost/date_time/gregorian/greg_date.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

bool compareCategories(const FinanceModel::Category& a, const FinanceModel::Category& b) 
{ 
    return a.name < b.name; 
} 

//Clone of InboxView
class CategoriesView : public Wt::WContainerWidget
{
public:
   CategoriesView(FinanceModel& model)
   : mModel(model)
   {
      Wt::WPushButton* addButton = new Wt::WPushButton("Add");
      addButton->clicked().connect(std::bind(&CategoriesView::onAddClick, this));
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

      auto items = mModel.getCategories();
      std::sort(items.begin(), items.end(), compareCategories);

      for(std::size_t i = 0; i < items.size(); ++i)
      {
         createCellAtRow(i, items[i]);
      }
   }

   void onItemDoubleClick(Wt::WTableCell* cell, const std::string& itemId)
   {
      std::function<void(std::string)> elementModifiedFunc = [=] (std::string a) {
         static_cast<Wt::WText*>(cell->widget(0))->setText(a);
         commitItemEdit(itemId, a);
      };

      CommonDialogManager::showOneLineDialog("Edit", "Name", static_cast<Wt::WText*>(cell->widget(0))->text().narrow(), elementModifiedFunc);
   }

   void onClick(Wt::WMouseEvent ev, Wt::WTableCell* cell, const std::string& itemId)
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

   void commitItemEdit(const std::string& itemId, const std::string& newText)
   {
      mModel.renameCategory(itemId, newText);
   }

   void commitItemDelete(const std::string& itemId)
   {
      mModel.eraseCategory(itemId);
   }

   materia::Id commitItemAdd(const std::string& text)
   {
      return mModel.addCategory(text);
   }

   void onAddClick()
   {
      std::function<void(std::string)> elementAddedFunc = [=] (std::string a) {
         auto id = commitItemAdd(a);
         createCellAtRow(mTable->rowCount(), {id, a});
      };

      CommonDialogManager::showOneLineDialog("Add", "Name", "", elementAddedFunc);
   }

   void createCellAtRow(const int row, const FinanceModel::Category& item)
   {
      auto cell = mTable->elementAt(row, 0);
      auto text = new Wt::WText(item.name);
      text->setMargin(Wt::WLength(50));
      cell->addWidget(std::unique_ptr<Wt::WText>(text));

      cell->clicked().connect(std::bind(&CategoriesView::onClick, this, std::placeholders::_1, cell, item.id));
      cell->doubleClicked().connect(std::bind(&CategoriesView::onItemDoubleClick, this, cell, item.id));
   }

private:
   Wt::WTable* mTable;
   FinanceModel& mModel;
};

//------------------------------------------------------------------------------------------------------------------------------

class DateCtrl : public Wt::WLabel
{
public:
   using TDateType = boost::gregorian::date;

   boost::signals2::signal<void()> OnChanged;

   DateCtrl(const TDateType& init)
   {
      setStyleClass("DateCtrl");
      setValue(init);

      clicked().connect(std::bind(&DateCtrl::showEditDialog, this));
   }

   void setValue(const TDateType& value)
   {
      mVal = value;

      const std::locale fmt(std::locale::classic(), new boost::gregorian::date_facet("%d/%m/%Y"));

      std::ostringstream os;
      os.imbue(fmt);
      os << value;
      
      setText(os.str());
   }

   TDateType getValue() const
   {
      return mVal;
   }

private:
   void showEditDialog()
   {
      std::function<void(const TDateType)> cb = [=] (auto newVal) {
         setValue(newVal);
         OnChanged();
      };

      CommonDialogManager::queryDate(mVal, cb);
   }

   TDateType mVal;
};

time_t to_time_t(const boost::gregorian::date& date )
{
	using namespace boost::posix_time;
	static ptime epoch(boost::gregorian::date(1970, 1, 1));
	time_duration::sec_type secs = (ptime(date,seconds(0)) - epoch).total_seconds();
	return time_t(secs);
}

class EventsView : public Wt::WContainerWidget
{
public:
   EventsView(FinanceModel& model)
   : mModel(model)
   {
      auto gb = new Wt::WGroupBox();
      addWidget(std::unique_ptr<Wt::WGroupBox>(gb));

      Wt::WPushButton* addButton = new Wt::WPushButton("Add");
      addButton->clicked().connect(std::bind(&EventsView::onAddButtonClicked, this));
      addButton->addStyleClass("btn-primary");
      addButton->setMargin(15, Wt::Side::Right);
      gb->addWidget(std::unique_ptr<Wt::WPushButton>(addButton));

      auto now = boost::gregorian::date(boost::gregorian::day_clock::local_day());

      gb->addWidget(std::make_unique<Wt::WLabel>("Showing events from "));
      mFrom = gb->addWidget(std::make_unique<DateCtrl>(now - boost::gregorian::date_duration(5)));
      mFrom->OnChanged.connect(std::bind(&EventsView::refreshTable, this));

      gb->addWidget(std::make_unique<Wt::WLabel>(" to "));
      mTo = gb->addWidget(std::make_unique<DateCtrl>(now));
      mTo->OnChanged.connect(std::bind(&EventsView::refreshTable, this));

      mTable = addNew<Wt::WTable>();
      mTable->setWidth(Wt::WLength("100%"));
      mTable->addStyleClass("table-bordered");
      mTable->addStyleClass("table-hover");
      mTable->addStyleClass("table-striped");
   }

private:

   void refreshTable()
   {
      auto categories = mModel.getCategories();

      mTable->clear();

      auto events = mModel.loadEvents(to_time_t(mFrom->getValue()), to_time_t(mTo->getValue()));

      for(auto e : events)
      {
         auto catPos = materia::find_by_id(categories, e.categoryId);
         auto catName = catPos == categories.end() ? e.categoryId.getGuid() : catPos->name;

         auto row = mTable->rowCount();
         mTable->elementAt(row, 0)->addNew<Wt::WLabel>(catName);
         mTable->elementAt(row, 1)->addNew<Wt::WLabel>(std::to_string(e.amountOfEuroCents));
         mTable->elementAt(row, 2)->addNew<Wt::WLabel>(std::to_string(e.timestamp));
         mTable->elementAt(row, 3)->addNew<Wt::WLabel>(e.details);
      }
   }

   void onAddButtonClicked()
   {
      auto cats = mModel.getCategories();

      auto d = CommonDialogManager::createCustomDialog("Finance event view", FinanceModel::Event{});
      
      d->addComboBox("Category", 
         cats, 
         cats.begin(), 
         [](auto x){return x.name;}, 
         [](FinanceModel::Event& obj, const FinanceModel::Category& selected){obj.categoryId = selected.id;}
         );

      d->addLineEdit("Details", &FinanceModel::Event::details);
      d->addDateEdit("Date", &FinanceModel::Event::timestamp, std::time(0));
      d->addCurrencyEdit("Amount", &FinanceModel::Event::amountOfEuroCents);

      d->onResult.connect([](auto e) { mModel.addEvent(e); refreshTable(); });
      d->show();
   }

   DateCtrl* mFrom;
   DateCtrl* mTo;
   Wt::WTable* mTable;
   FinanceModel& mModel;
};

//------------------------------------------------------------------------------------------------------------------------------

FinanceView::FinanceView(FinanceModel& model)
: mFinance(model)
{
   setMargin(15, Wt::Side::Left);

   mCategoriesBtn = new Wt::WPushButton("Categories");
   mCategoriesBtn->setStyleClass("btn-primary btn-lg");
   mCategoriesBtn->clicked().connect(this, &FinanceView::showCategories);
   
   auto eventsGroup = new Wt::WGroupBox;
   eventsGroup->addWidget(std::unique_ptr<Wt::WPushButton>(mCategoriesBtn));

   eventsGroup->addWidget(std::unique_ptr<Wt::WWidget>(new EventsView(model)));
   addWidget(std::unique_ptr<Wt::WGroupBox>(eventsGroup));
}

void FinanceView::showCategories()
{
   Dialog* dlg = new Dialog("Categories View", std::make_unique<CategoriesView>(mFinance));
   dlg->show();
}