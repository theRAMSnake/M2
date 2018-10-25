#include "CalendarView.hpp"
#include "WtConverters.hpp"
#include "CommonDialogManager.hpp"
#include <Common/Id.hpp>
#include <Wt/WDialog.h>
#include <Wt/WLineEdit.h>
#include <Wt/WGroupBox.h>
#include <Wt/WTimeEdit.h>
#include <Wt/WDateEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WCalendar.h>
#include <Wt/WCssDecorationStyle.h>
#include <boost/date_time/gregorian/greg_date.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

class CalendarItemDialog : public Wt::WDialog
{
public:
   typedef std::function<void(const CalendarModel::Item&)> TOnOkCallback;
   CalendarItemDialog(const CalendarModel::Item& src, TOnOkCallback cb)
   {
      setWidth(Wt::WLength("50%"));
      auto title = new Wt::WLineEdit();
      title->setText(src.text);
      contents()->addWidget(std::unique_ptr<Wt::WLineEdit>(title));

      auto gb = contents()->addWidget(Wt::cpp14::make_unique<Wt::WGroupBox>());

      gb->addStyleClass("row justify-content-center");

      auto time = gb->addWidget(Wt::cpp14::make_unique<Wt::WTimeEdit>());
      time->setTime(timestampToWtTime(src.timestamp));
      time->setWidth(Wt::WLength("15%"));
      time->addStyleClass("col-md-6");
      time->setMargin(Wt::WLength("35%"), Wt::Side::Left);

      auto date = gb->addWidget(Wt::cpp14::make_unique<Wt::WDateEdit>());
      date->setDate(timestampToWtDate(src.timestamp));
      date->setWidth(Wt::WLength("15%"));
      date->addStyleClass("col-md-6");

      auto ok = new Wt::WPushButton("Accept");
      ok->setDefault(true);
      ok->clicked().connect(std::bind([=]() {
          if (time->validate() == Wt::ValidationState::Valid && 
            date->validate() == Wt::ValidationState::Valid &&
            !title->text().empty())
          {
            accept();
          }
      }));
      footer()->addWidget(std::unique_ptr<Wt::WPushButton>(ok));

      auto cancel = new Wt::WPushButton("Cancel");
      footer()->addWidget(std::unique_ptr<Wt::WPushButton>(cancel));
      cancel->clicked().connect(this, &Wt::WDialog::reject);

      rejectWhenEscapePressed();

      finished().connect(std::bind([=]() {
        if (result() == Wt::DialogCode::Accepted)
        {
            CalendarModel::Item resultItem(src);
            resultItem.text = title->text().narrow();
            resultItem.timestamp = WtDateTimeToTimestamp(date->date(), time->time());

            cb(resultItem);
        }

        delete this;
      }));
   }
};

class MateriaCalendar : public Wt::WCalendar
{
public:
    MateriaCalendar(CalendarModel& calendar)
    : mCalendar(calendar)
    {
        populate();

        currentPageChanged().connect([=]{populate();refresh();});
    }

    std::vector<CalendarModel::Item> getItems(const Wt::WDate& date)
    {
        std::vector<CalendarModel::Item> result;

        int timestampFrom = WtDateToTimeStamp(date);
        int timestampTo = WtDateToTimeStamp(date.addDays(1)) - 1;

        for(auto x : mItems)
        {
            if(x.timestamp >= timestampFrom && x.timestamp <= timestampTo)
            {
                result.push_back(x);
            }
        }

        return result;
    }

    void addItem(const CalendarModel::Item& item)
    {
        mItems.push_back(item);
    }

    void removeItem(const materia::Id& id)
    {
       auto iter = materia::find_by_id(mItems, id);
       if(iter != mItems.end())
       {
          mItems.erase(iter);
          hide();
          show();
       }
    }

    virtual ~MateriaCalendar(){}

private:
    void populate()
    {
        mItems.clear();

        auto firstDay = boost::gregorian::date(currentYear(), static_cast<boost::date_time::months_of_year>(currentMonth()), 1);

        auto from = boost::posix_time:: to_time_t(boost::posix_time::ptime(firstDay - boost::gregorian::date_duration(6)));
        auto to = boost::posix_time:: to_time_t(boost::posix_time::ptime(firstDay + boost::gregorian::date_duration(37)));

        mItems = mCalendar.query(from, to);
    }

    virtual Wt::WWidget* renderCell(Wt::WWidget *widget, const Wt::WDate &date)
    {
       auto parentResult = Wt::WCalendar::renderCell(widget, date);

       if(parentResult != nullptr)
       {
          if(!getItems(date).empty())
          {
              Wt::WCssDecorationStyle style;
              style.setBackgroundColor(Wt::StandardColor::DarkGreen);
              static_cast<Wt::WWebWidget*>(parentResult)->setDecorationStyle (style);
          }
          else
          {
              Wt::WCssDecorationStyle style;
              static_cast<Wt::WWebWidget*>(parentResult)->setDecorationStyle (style);
          }
       }   

       return parentResult;
    }

    CalendarModel& mCalendar;
    std::vector<CalendarModel::Item> mItems;
};

struct calendarByTimestampCmp
{
    bool operator()(const CalendarModel::Item& lhs, const CalendarModel::Item& rhs) const
    {
        return lhs.timestamp < rhs.timestamp;
    }
};

class CalendarItemList : public Wt::WGroupBox
{
public:
    enum class DisplayMode
    {
        Countdown,
        Time
    };

    CalendarItemList(const std::string& name, const DisplayMode displayMode)
    : Wt::WGroupBox(name)
    , mDisplayMode(displayMode)
    {

    }

    void addItem(const CalendarModel::Item& item)
    {
        auto w = createWidget(item);
        mItems.insert(std::make_pair(item, w));
        addWidget(std::unique_ptr<Wt::WWidget>(w));
    }

    bool removeItem(const CalendarModel::Item& item)
    {
       auto iter = mItems.find(item);
       if(iter != mItems.end())
       {
          iter->second->hide();
          mItems.erase(iter);

          return true;
       }

       return false;
    }

    void updateItemsOrdering()
    {
       if(mItems.size() > 1)
       {
         clearWidgets();
         for(auto x = mItems.begin(); x != mItems.end(); ++x)
         {
            auto w = createWidget(x->first);
            addWidget(std::unique_ptr<Wt::WWidget>(w));
            x->second = w;
         }
       }
    }

    virtual void clearAll()
    {
       clearWidgets();
       mItems.clear();
       setTitle(""); 
    }

    Wt::Signal<CalendarModel::Item>& onItemCtrlClicked()
    {
       return mOnItemCtrlClicked;
    }

    Wt::Signal<CalendarModel::Item>& onItemDblClicked()
    {
       return mOnItemDblClicked;
    }

private:
   void clearWidgets()
   {
      for(auto x = mItems.begin(); x != mItems.end(); ++x)
      {
         //This should be remove instead, but since remove is crashing and I do not know why - this is also ok.
         //It may crash Wt after some usage time, but that is ok - no data lost and autorestart is there.
         x->second->hide();
         x->second = nullptr;
      }
   } 

   Wt::WWidget* createWidget(const CalendarModel::Item& item)
   {
      std::string text;

      switch(mDisplayMode)
      {
         case DisplayMode::Time:
         {
               auto ptime = boost::posix_time::from_time_t(item.timestamp);

               text = boost::lexical_cast<std::string>(ptime.time_of_day(). hours()) + ":" +
                  boost::lexical_cast<std::string>(ptime.time_of_day().minutes());

               if(boost::lexical_cast<std::string>(ptime.time_of_day().minutes()).size() < 2)
               {
                  text += "0";
               }        
         }
         break;

         case DisplayMode::Countdown:
         {
               auto curTime = boost::posix_time::from_time_t(WtDateTimeToTimestamp(
                  Wt::WDateTime::currentDateTime().date(), Wt::WDateTime::currentDateTime().time()));
               auto itemTime = boost::posix_time::from_time_t(item.timestamp);

               auto diff = itemTime - curTime;

               text = boost::lexical_cast<std::string>(diff.hours() / 24);
         }
         break;
      }

      auto textWidget = new Wt::WText(text + " " + item.text + "<br></br><br></br>");

      textWidget->clicked().connect([=]( Wt::WMouseEvent x)
        {
            if(x.modifiers().test(Wt::KeyboardModifier::Control))
            {
               mOnItemCtrlClicked(item);
            }  
        });

      textWidget->doubleClicked().connect([=]( Wt::WMouseEvent x)
        {
            mOnItemDblClicked(item); 
        });

      Wt::WCssDecorationStyle style;
      style.font().setSize(Wt::FontSize::Large);
      textWidget->setDecorationStyle (style);

      return textWidget;
   }

   Wt::Signal<CalendarModel::Item> mOnItemCtrlClicked;
   Wt::Signal<CalendarModel::Item> mOnItemDblClicked;
   std::multimap<CalendarModel::Item, Wt::WWidget*, calendarByTimestampCmp> mItems;
   const DisplayMode mDisplayMode;
};

CalendarView::CalendarView(CalendarModel& calendar)
: mCalendar(calendar)
{
   mCalendarWidget = new MateriaCalendar(mCalendar);
   mCalendarWidget->clicked().connect([=]{onCalendarClicked();} );
   addWidget(std::unique_ptr<Wt::WCalendar>(mCalendarWidget));

   mDateCalendar = addWidget(std::unique_ptr<CalendarItemList>(new CalendarItemList("", CalendarItemList::DisplayMode::Time)));
   mDateCalendar->onItemCtrlClicked().connect([=](CalendarModel::Item x){initiateItemDelete(x);} );
   mDateCalendar->onItemDblClicked().connect([=](CalendarModel::Item x){initiateItemEdit(x);} );
   mNextCalendar = addWidget(std::unique_ptr<CalendarItemList>(new CalendarItemList("Upcoming", CalendarItemList::DisplayMode::Countdown)));
   mNextCalendar->onItemCtrlClicked().connect([=](CalendarModel::Item x){initiateItemDelete(x);} );
   mNextCalendar->onItemDblClicked().connect([=](CalendarModel::Item x){initiateItemEdit(x);} );

   updateNextCalendar();

   mDateCalendar->clicked().connect([=]( Wt::WMouseEvent x)
   {
      if(x.modifiers().test(Wt::KeyboardModifier::Shift) && !mCalendarWidget->selection().empty())
      {
            initiateItemAdd(*mCalendarWidget->selection().begin());
      }  
   });
}

void CalendarView::initiateItemAdd(const Wt::WDate date)
{
   CalendarModel::Item item; 
   item.timestamp = WtDateTimeToTimestamp(date, Wt::WTime(9, 0));
   CalendarItemDialog* dlg = new CalendarItemDialog(item, [=](const CalendarModel::Item& result) 
   {
      materia::Id id = mCalendar.add(result);

      if(id != materia::Id::Invalid)
      {
         CalendarModel::Item item {result};
         item.id = id;

         mCalendarWidget->addItem(item);

         mDateCalendar->setTitle(date.toString());
         mDateCalendar->addItem(item);
         mDateCalendar->updateItemsOrdering();

         if(item.timestamp < mNextCalendarHighestItemTimestamp)
         {
            updateNextCalendar();    
         }
      }
   });

   dlg->show();
}

void CalendarView::initiateItemDelete(const CalendarModel::Item item)
{
   std::function<void()> elementDeletedFunc = [=] () {
      mCalendar.erase(item.id);

      mDateCalendar->removeItem(item);
      if(mNextCalendar->removeItem(item))
      {
         updateNextCalendar();
      }
      mCalendarWidget->removeItem(item.id);

      auto items = mCalendarWidget->getItems(timestampToWtDate(item.timestamp));
      if(items.empty())
      {
         mDateCalendar->clearAll();
      }

      };

   CommonDialogManager::showConfirmationDialog("Delete it?", elementDeletedFunc);
}

void CalendarView::initiateItemEdit(const CalendarModel::Item item)
{
   const CalendarModel::Item prevItem(item);
   CalendarItemDialog* dlg = new CalendarItemDialog(item, [=](const CalendarModel::Item& result) 
   {
      mCalendar.replace(result);
      {
         mCalendarWidget->removeItem(prevItem.id);
         mCalendarWidget->addItem(result);
         mCalendarWidget->refresh();

         mDateCalendar->removeItem(prevItem);
         Wt::WDate date = *mCalendarWidget->selection().begin();
         if(timestampToWtDate(result.timestamp) == date)
         {
            mDateCalendar->addItem(result);
            mDateCalendar->updateItemsOrdering();
         }

         if(mNextCalendar->removeItem(prevItem))
         {
            updateNextCalendar();
         }
      }
   });

   dlg->show();
}

void CalendarView::onCalendarClicked()
{
   Wt::WDate date = *mCalendarWidget->selection().begin();
   mDateCalendar->clearAll();
   auto items = mCalendarWidget->getItems(date);
   if(items.empty())
   {
      initiateItemAdd(date);
   }
   else
   {
      mDateCalendar->setTitle(date.toString());
      for(auto x : items)
      {
            mDateCalendar->addItem(x);
      }

      mDateCalendar->updateItemsOrdering();
   }
}

void CalendarView::updateNextCalendar()
{
   mNextCalendar->clearAll();
   mNextCalendar->setTitle("Upcoming");

   for(auto x : mCalendar.next(
      WtDateTimeToTimestamp(Wt::WDateTime::currentDateTime().date(), Wt::WDateTime::currentDateTime().time()), 5))
   {
      mNextCalendar->addItem(x);
      mNextCalendarHighestItemTimestamp = x.timestamp;
   }
}