#include "FinanceView.hpp"
#include <Wt/WGroupBox.h>
#include <Wt/WPushButton.h>
#include <Wt/WDialog.h>
#include <Wt/WLabel.h>
#include <Wt/WTable.h>
#include <Wt/WCssDecorationStyle.h>
#include <Wt/WDateEdit.h>
#include <Wt/WMenu.h>
#include <Wt/WTextArea.h>
#include <Wt/WStackedWidget.h>
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
      
      auto gb = new Wt::WGroupBox();
      addWidget(std::unique_ptr<Wt::WGroupBox>(gb));

      gb->addWidget(std::unique_ptr<Wt::WPushButton>(addButton));
      
      mTable = new Wt::WTable();
      mTable->setWidth(Wt::WLength("100%"));
      mTable->setMargin(25, Wt::Side::Top);
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

   DateCtrl(const TDateType& init, bool readonly)
   {
      setStyleClass("DateCtrl");
      setValue(init);

      if(!readonly)
      {
         clicked().connect(std::bind(&DateCtrl::showEditDialog, this));
      }
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

class MonthSelectCtrl : public Wt::WLineEdit
{
public:
   using TDateType = boost::gregorian::date;

   boost::signals2::signal<void()> OnChanged;

   MonthSelectCtrl(const TDateType& init)
   {
      setValue(init);
      setInputMask("09/9999;_");

      changed().connect(std::bind([=](){
         auto str = text().narrow();
         std::istringstream iss (str);

         unsigned int month = 0;
         unsigned int year = 0;
         char separator;
         iss >> month >> separator >> year;

         setValue(boost::gregorian::date(year, month, 1));
      }));
   }

   void setValue(const TDateType& value)
   {
      mVal = value;

      const std::locale fmt(std::locale::classic(), new boost::gregorian::date_facet("%m/%Y"));

      std::ostringstream os;
      os.imbue(fmt);
      os << value;
      
      setText(os.str());
      OnChanged();
   }

   TDateType getValue() const
   {
      return mVal;
   }

private:
   TDateType mVal;
};

time_t to_time_t(const boost::gregorian::date& date)
{
	using namespace boost::posix_time;
	static ptime epoch(boost::gregorian::date(1970, 1, 1));
	time_duration::sec_type secs = (ptime(date,seconds(0)) - epoch).total_seconds();
	return time_t(secs);
}

boost::gregorian::date alignToStartOfMonth(const boost::gregorian::date& date)
{
   return boost::gregorian::date(date.year(), date.month(), 1);
}

class MonthlyView : public Wt::WContainerWidget
{
public:
   MonthlyView(FinanceModel& model)
   : mModel(model)
   {
      auto gb = new Wt::WGroupBox();
      addWidget(std::unique_ptr<Wt::WGroupBox>(gb));

      auto now = boost::gregorian::date(boost::gregorian::day_clock::local_day());

      gb->addWidget(std::make_unique<Wt::WLabel>("Showing results for "));
      auto month = gb->addWidget(std::make_unique<MonthSelectCtrl>(now));
      month->OnChanged.connect(std::bind([=]() {
         refreshTable(month->getValue());
      }));
      month->setWidth("20%");
      month->setInline(true);

      mTable = addNew<Wt::WTable>();
      mTable->setWidth(Wt::WLength("100%"));
      mTable->addStyleClass("table-bordered");
      mTable->addStyleClass("table-hover");
      mTable->addStyleClass("table-striped");
      mTable->decorationStyle().font().setSize(Wt::WFont::Size::Medium);
      mTable->setMargin(25, Wt::Side::Top);

      refreshTable(now);
   }

private:
   void refreshTable(boost::gregorian::date date)
   {
      mTable->clear();

      auto events = mModel.loadEvents(to_time_t(alignToStartOfMonth(date)),
         to_time_t(date.end_of_month() + boost::gregorian::date_duration(1)));

      auto categories = mModel.getCategories();

      unsigned int total = 0;
      std::map<materia::Id, unsigned int> amountByCategory;

      for(auto e : events)
      {
         amountByCategory[e.categoryId] += e.amountOfEuroCents;
         total += e.amountOfEuroCents;
      }

      for(auto d : amountByCategory)
      {
         auto catPos = materia::find_by_id(categories, d.first);
         auto catName = catPos == categories.end() ? d.first.getGuid() : catPos->name;

         auto row = mTable->rowCount();
         mTable->elementAt(row, 0)->addNew<Wt::WLabel>(catName);
         mTable->elementAt(row, 1)->addNew<Wt::WLabel>(currencyToString(d.second));
      }

      auto row = mTable->rowCount();
      mTable->elementAt(row, 0)->addNew<Wt::WLabel>("<b>Total</b>");
      mTable->elementAt(row, 1)->addNew<Wt::WLabel>("<b>" + currencyToString(total) + "</b>");
   }

   Wt::WTable* mTable;
   FinanceModel& mModel;
};

class AnnualView : public Wt::WContainerWidget
{
public:
   AnnualView(FinanceModel& model)
   : mModel(model)
   {
      auto gb = new Wt::WGroupBox();
      addWidget(std::unique_ptr<Wt::WGroupBox>(gb));

      Wt::WPushButton* refreshButton = new Wt::WPushButton("Refresh");
      refreshButton->clicked().connect(std::bind(&AnnualView::refreshTable, this));
      refreshButton->addStyleClass("btn-primary");

      gb->addWidget(std::unique_ptr<Wt::WPushButton>(refreshButton));

      mTable = addNew<Wt::WTable>();
      mTable->setWidth(Wt::WLength("100%"));
      mTable->addStyleClass("table-bordered");
      mTable->addStyleClass("table-hover");
      mTable->addStyleClass("table-striped");
      mTable->decorationStyle().font().setSize(Wt::WFont::Size::Medium);
      mTable->setMargin(25, Wt::Side::Top);

      //refreshTable();
   }

private:
   void refreshTable()
   {
      unsigned int grandTotal = 0;
      std::map<materia::Id, std::map<boost::gregorian::date, unsigned int>> amountByCategory;
      std::map<boost::gregorian::date, int> months;
      std::map<boost::gregorian::date, unsigned int> total_per_month;
      auto categories = mModel.getCategories();

      mTable->clear();

      //table not working
      auto date = alignToStartOfMonth(boost::gregorian::date(boost::gregorian::day_clock::local_day()));

      for(int i = 0; i < 12; i++)
      {
         months[date] = i;

         auto events = mModel.loadEvents(to_time_t(date),
            to_time_t(date.end_of_month() + boost::gregorian::date_duration(1)));

         for(auto e : events)
         {
            amountByCategory[e.categoryId][date] += e.amountOfEuroCents;
            grandTotal += e.amountOfEuroCents;
            total_per_month[date] += e.amountOfEuroCents;
         }

         const std::locale fmt(std::locale::classic(), new boost::gregorian::date_facet("%m/%Y"));

         std::ostringstream os;
         os.imbue(fmt);
         os << date;
         
         mTable->elementAt(0, i + 1)->addNew<Wt::WLabel>(os.str());

         date -= boost::gregorian::months(1);
      }

      for(auto d : amountByCategory)
      {
         auto catPos = materia::find_by_id(categories, d.first);
         auto catName = catPos == categories.end() ? d.first.getGuid() : catPos->name;

         auto row = mTable->rowCount();
         mTable->elementAt(row, 0)->addNew<Wt::WLabel>("<b>" + catName + "</b>");

         unsigned int total = 0;
         for(auto m : d.second)
         {
            total += m.second;
            mTable->elementAt(row, months[m.first] + 1)->addNew<Wt::WLabel>(currencyToString(m.second));
         }

         mTable->elementAt(row, 13)->addNew<Wt::WLabel>("<b>" + currencyToString(total) + "</b>");
      }

      auto row = mTable->rowCount();
      mTable->elementAt(row, 0)->addNew<Wt::WLabel>("<b>Total</b>");
      mTable->elementAt(row, 13)->addNew<Wt::WLabel>("<b>" + currencyToString(grandTotal) + "</b>");

      for(auto m : total_per_month)
      {
         mTable->elementAt(row, months[m.first] + 1)->addNew<Wt::WLabel>("<b>" + currencyToString(m.second) + "</b>");
      }
   }

   Wt::WTable* mTable;
   FinanceModel& mModel;
};

class EventsView : public Wt::WContainerWidget
{
public:
   struct WrappedEvent : public FinanceModel::Event
   {
      bool isAnnual;

      WrappedEvent(const FinanceModel::Event e)
      {
         amountOfEuroCents = e.amountOfEuroCents;
         categoryId = e.categoryId;
         details = e.details;
         eventId = e.eventId;
         isAnnual = false;
         timestamp = e.timestamp;
      }
   };

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
      mFrom = gb->addWidget(std::make_unique<DateCtrl>(now - boost::gregorian::date_duration(5), false));
      mFrom->OnChanged.connect(std::bind(&EventsView::refreshTable, this));

      gb->addWidget(std::make_unique<Wt::WLabel>(" to "));
      mTo = gb->addWidget(std::make_unique<DateCtrl>(now, false));
      mTo->OnChanged.connect(std::bind(&EventsView::refreshTable, this));

      mTable = addNew<Wt::WTable>();
      mTable->setWidth(Wt::WLength("100%"));
      mTable->addStyleClass("table-bordered");
      mTable->addStyleClass("table-hover");
      mTable->addStyleClass("table-striped");
      

      refreshTable();
   }

private:

   void refreshTable()
   {
      auto categories = mModel.getCategories();

      mTable->clear();
      mTable->setMargin(25, Wt::Side::Top);

      auto events = mModel.loadEvents(to_time_t(mFrom->getValue()), to_time_t(mTo->getValue() + boost::gregorian::date_duration(1)));

      mTable->elementAt(0, 0)->addNew<Wt::WLabel>("<b>Category</b>");
      mTable->elementAt(0, 1)->addNew<Wt::WLabel>("<b>Amount</b>");
      mTable->elementAt(0, 2)->addNew<Wt::WLabel>("<b>Date</b>");
      mTable->elementAt(0, 3)->addNew<Wt::WLabel>("<b>Details</b>");
      mTable->decorationStyle().font().setSize(Wt::WFont::Size::Medium);

      for(auto e : events)
      {
         auto catPos = materia::find_by_id(categories, e.categoryId);
         auto catName = catPos == categories.end() ? e.categoryId.getGuid() : catPos->name;

         auto row = mTable->rowCount();
         mTable->elementAt(row, 0)->addNew<Wt::WLabel>(catName);
         auto activeElement = mTable->elementAt(row, 1)->addNew<Wt::WLabel>(currencyToString(e.amountOfEuroCents));
         mTable->elementAt(row, 2)->addNew<DateCtrl>(timestampToGregorian(e.timestamp), true);
         mTable->elementAt(row, 3)->addNew<Wt::WLabel>(e.details);

         activeElement->doubleClicked().connect(std::bind([=]() 
         {
            CommonDialogManager::showBinaryChoiseDialog("Edit", "Erase", 
            [=]() 
            {
               WrappedEvent w(e);

               auto d = createDialog(w);
               d->onResult.connect([=](auto x) { mModel.modifyEvent(x); refreshTable(); });
               d->show();
            },
            [=]() 
            {
               mModel.deleteEvent(e.eventId);
               refreshTable();
            }
            );
         }));
      }
   }

   void onAddButtonClicked()
   {
      auto d = createDialog(WrappedEvent({}));
      d->onResult.connect([=](auto e) 
      {
         if(e.isAnnual)
         {
            for(int i = 0; i < 12; ++i)
            {
               auto splitted = e;
               splitted.details += " " + std::to_string(i+1) + "/12";
               splitted.amountOfEuroCents = e.amountOfEuroCents / 12;
               splitted.timestamp = to_time_t(timestampToGregorian(e.timestamp) + boost::gregorian::months(i));

               mModel.addEvent(splitted);
            }
         } 
         else
         {
            mModel.addEvent(e);
         }
          
         refreshTable(); 
      });
      d->show();
   }

   CustomDialog<WrappedEvent>* createDialog(const WrappedEvent ev)
   {
      auto cats = mModel.getCategories();

      auto d = CommonDialogManager::createCustomDialog("Finance event view", ev);
      
      d->addComboBox("Category", 
         cats, 
         cats.begin(), 
         [](auto x){return x.name;}, 
         [](WrappedEvent& obj, const FinanceModel::Category& selected){obj.categoryId = selected.id;}
         );

      d->addLineEdit("Details", &WrappedEvent::details);
      d->addDateEdit("Date", &WrappedEvent::timestamp, std::time(0));
      d->addCurrencyEdit("Amount", &WrappedEvent::amountOfEuroCents);
      d->addCheckbox("Annual", &WrappedEvent::isAnnual);

      return d;
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

   auto container = Wt::cpp14::make_unique<Wt::WContainerWidget>();
   auto contents = Wt::cpp14::make_unique<Wt::WStackedWidget>();

   Wt::WMenu *menu = container->addNew<Wt::WMenu>(contents.get());
   menu->setStyleClass("nav nav-pills nav-stacked");
   menu->setWidth(150);

   menu->addItem("Monthly", std::unique_ptr<Wt::WWidget>(new MonthlyView(model)));
   menu->addItem("Annual", std::unique_ptr<Wt::WWidget>(new AnnualView(model)));
   menu->addItem("Events", std::unique_ptr<Wt::WWidget>(new EventsView(model)));
   menu->addItem("Categories", std::make_unique<CategoriesView>(mFinance));

   container->addWidget(std::move(contents));
   addWidget(std::move(container));
}