#include "ActionsView.hpp"
#include "CommonDialogManager.hpp"
#include <Common/ZmqPbChannel.hpp>
#include <Common/PortLayout.hpp>
#include <Wt/WTree.h>
#include <Wt/WTreeNode.h>
#include <Wt/WCssDecorationStyle.h>
#include <Wt/WTextEdit.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WDialog.h>
#include <Wt/WCheckBox.h>
#include <Wt/WGroupBox.h>
#include <Wt/WCalendar.h>
#include <Wt/WTimeEdit.h>
#include <Wt/WDateEdit.h>
#include "boost/date_time/gregorian/gregorian.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>

class ActionsTreeNodeEditDialog : public Wt::WDialog
{
public:
   typedef std::function<void(const std::string&, const std::string&)> TOnOkCallback;
   ActionsTreeNodeEditDialog(const std::string& title, const std::string& desc, TOnOkCallback cb)
   {
      setWidth(Wt::WLength("75%"));
      mTitle = new Wt::WLineEdit(title);
      contents()->addWidget(std::unique_ptr<Wt::WLineEdit>(mTitle));

      mDesc = new Wt::WTextEdit();
      mDesc->setHeight(500);
      mDesc->setConfigurationSetting("branding", false);
      mDesc->setText(desc);
      
      contents()->addWidget(std::unique_ptr<Wt::WTextEdit>(mDesc));

      auto ok = new Wt::WPushButton("Accept");
      ok->setDefault(true);
      ok->clicked().connect(std::bind([=]() {
        accept();
      }));
      footer()->addWidget(std::unique_ptr<Wt::WPushButton>(ok));

      auto cancel = new Wt::WPushButton("Cancel");
      footer()->addWidget(std::unique_ptr<Wt::WPushButton>(cancel));
      cancel->clicked().connect(this, &Wt::WDialog::reject);

      rejectWhenEscapePressed();

      finished().connect(std::bind([=]() {
        if (result() == Wt::DialogCode::Accepted)
        {
            cb(mTitle->text().narrow(), mDesc->text().narrow());
        }

        delete this;
      }));
   }

private:
   Wt::WLineEdit* mTitle;
   Wt::WTextEdit* mDesc;
};

class ActionsTreeNode : public Wt::WTreeNode
{
public:
   ActionsTreeNode(const actions::ActionInfo& actionInfo, actions::ActionsService_Stub& actionsService)
   : Wt::WTreeNode(actionInfo.title())
   , mActionInfo(actionInfo)
   , mActionsService(actionsService)
   {
      labelArea()->doubleClicked().connect(std::bind(&ActionsTreeNode::onDblClicked, this, std::placeholders::_1));
      labelArea()->clicked().connect(std::bind(&ActionsTreeNode::onClick, this, std::placeholders::_1));

      acceptDrops("node");
   }

   virtual void populate() override
   {
      actions::ActionsList result;
      mActionsService.GetChildren(nullptr, &mActionInfo.id(), &result, nullptr);

      for(int i = 0; i < result.list_size(); ++i)
      {
         addChildNode(Wt::cpp14::make_unique<ActionsTreeNode>(result.list(i), mActionsService));
      }
   }

   void reparent(const common::UniqueId& parentId, Wt::WTreeNode* newParentNode)
   {
      mActionInfo.mutable_parentid()->set_guid(parentId.guid());

      common::OperationResultMessage dummy;
      mActionsService.EditElement(nullptr, &mActionInfo, &dummy, nullptr);
      
      newParentNode->addChildNode(parentNode()->removeChildNode(this));
   }

   void dropEvent	(Wt::WDropEvent dropEvent)	
   {
      ActionsTreeNode* node = static_cast<ActionsTreeNode*>(dropEvent.source());
      node->reparent(mActionInfo.id(), this);
   }

   void setMoveMode(const bool moveMode)
   {
      if(moveMode)
      {
         labelArea()->setDraggable("node", nullptr, false, this);
      }
      else
      {
         labelArea()->unsetDraggable();
      }
      
      if(populated())
      {
         for(auto x : childNodes())
         {
            ActionsTreeNode* node = static_cast<ActionsTreeNode*>(x);
            node->setMoveMode(moveMode);
         }
      }
      
   }

private:
   void onDblClicked(Wt::WMouseEvent ev)
   {
      auto dlg = new ActionsTreeNodeEditDialog(
         mActionInfo.title(),
         mActionInfo.description(),
         std::bind(&ActionsTreeNode::onDialogOk, this, std::placeholders::_1, std::placeholders::_2));
      dlg->show();
   }

   void onDialogOk(const std::string& title, const std::string& desc)
   {
      label()->setText(title);

      mActionInfo.set_title(title);
      mActionInfo.set_description(desc);

      common::OperationResultMessage dummy;
      mActionsService.EditElement(nullptr, &mActionInfo, &dummy, nullptr);
   }

   void onClick(Wt::WMouseEvent ev)
   {
      if(ev.modifiers().test(Wt::KeyboardModifier::Control))
      {
          std::function<void()> elementDeletedFunc = [=] () {
            common::OperationResultMessage dummy;
            mActionsService.DeleteElement(nullptr, &mActionInfo.id(), &dummy, nullptr);

            parentNode()->removeChildNode(this);
            };

          CommonDialogManager::showConfirmationDialog("Delete it?", elementDeletedFunc);
      }
      else if(ev.modifiers().test(Wt::KeyboardModifier::Shift))
      {
          auto dlg = new ActionsTreeNodeEditDialog(
          "",
          "",
          [=](const std::string& title, const std::string& desc)
          {
              actions::ActionInfo newItem;
              newItem.set_title(title);
              newItem.set_description(desc);
              newItem.mutable_parentid()->set_guid(mActionInfo.id().guid());

              common::UniqueId* newId = new common::UniqueId;
              mActionsService.AddElement(nullptr, &newItem, newId, nullptr);

              newItem.set_allocated_id(newId);
              
              addChildNode(Wt::cpp14::make_unique<ActionsTreeNode>(newItem, mActionsService));
          });
          dlg->show();
      }
   }

   actions::ActionInfo mActionInfo;
   actions::ActionsService_Stub& mActionsService;
};

class ActionsRootTreeNode : public Wt::WTreeNode
{
public:
   ActionsRootTreeNode(actions::ActionsService_Stub& actionsService)
   : Wt::WTreeNode("Actions")
   , mActionsService(actionsService)
   {
      labelArea()->clicked().connect(std::bind(&ActionsRootTreeNode::onClick, this, std::placeholders::_1));
      acceptDrops("node");
   }

   virtual void populate() override
   {
      common::EmptyMessage dummy;
      actions::ActionsList result;
      mActionsService.GetParentlessElements(nullptr, &dummy, &result, nullptr);

      for(int i = 0; i < result.list_size(); ++i)
      {
         addChildNode(Wt::cpp14::make_unique<ActionsTreeNode>(result.list(i), mActionsService));
      }
   }

   virtual void dropEvent(Wt::WDropEvent dropEvent) override
   {
      ActionsTreeNode* node = dynamic_cast<ActionsTreeNode*>(dropEvent.source());
      if(node == nullptr)
      {
         std::cout << "\nshit\n";
      }
      else
      {
         std::cout << "\nnodeok\n";
      }
      node->reparent(common::UniqueId(), this);
   }

   void setMoveMode(const bool moveMode)
   {
      for(auto x : childNodes())
      {
         ActionsTreeNode* node = static_cast<ActionsTreeNode*>(x);
         node->setMoveMode(moveMode);
      }
   }

private:
   void onClick(Wt::WMouseEvent ev)
   {
      if(ev.modifiers().test(Wt::KeyboardModifier::Shift))
      {
         auto dlg = new ActionsTreeNodeEditDialog(
         "",
         "",
         [=](const std::string& title, const std::string& desc)
         {
            actions::ActionInfo newItem;
            newItem.set_title(title);
            newItem.set_description(desc);

            common::UniqueId* newId = new common::UniqueId;
            mActionsService.AddElement(nullptr, &newItem, newId, nullptr);

            newItem.set_allocated_id(newId);
            addChildNode(Wt::cpp14::make_unique<ActionsTreeNode>(newItem, mActionsService));
         });
         dlg->show();
      }
   }

   actions::ActionsService_Stub& mActionsService;
};

int WtDateTimeToTimestamp(const Wt::WDate& date, const Wt::WTime& time)
{
    return boost::posix_time::to_time_t(boost::posix_time::ptime(
        boost::gregorian::date(
            date.year(), 
            static_cast<boost::date_time::months_of_year>(date.month()), 
            date.day()),
            boost::posix_time::hours(time.hour()) +
            boost::posix_time::minutes(time.minute())));
}

int WtDateToTimeStamp(const Wt::WDate& date)
{
    return boost::posix_time::to_time_t(boost::posix_time::ptime(
        boost::gregorian::date(date.year(), static_cast<boost::date_time::months_of_year>(date.month()), date.day())));
}

Wt::WDate timestampToWtDate(const int& timestamp)
{
    return Wt::WDate(std::chrono::system_clock::from_time_t(timestamp));
}

Wt::WTime timestampToWtTime(const int& timestamp)
{
    return Wt::WDateTime(std::chrono::system_clock::from_time_t(timestamp)).time();
}

class CalendarItemDialog : public Wt::WDialog
{
public:
   typedef std::function<void(const calendar::CalendarItem&)> TOnOkCallback;
   CalendarItemDialog(const calendar::CalendarItem& src, TOnOkCallback cb)
   {
      setWidth(Wt::WLength("50%"));
      auto title = new Wt::WLineEdit();
      title->setText(src.text());
      contents()->addWidget(std::unique_ptr<Wt::WLineEdit>(title));

      auto gb = contents()->addWidget(Wt::cpp14::make_unique<Wt::WGroupBox>());

      gb->addStyleClass("row justify-content-center");

      auto time = gb->addWidget(Wt::cpp14::make_unique<Wt::WTimeEdit>());
      time->setTime(timestampToWtTime(src.timestamp()));
      time->setWidth(Wt::WLength("15%"));
      time->addStyleClass("col-md-6");
      time->setMargin(Wt::WLength("35%"), Wt::Side::Left);

      auto date = gb->addWidget(Wt::cpp14::make_unique<Wt::WDateEdit>());
      date->setDate(timestampToWtDate(src.timestamp()));
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
            calendar::CalendarItem resultItem(src);
            resultItem.set_text(title->text().narrow());
            resultItem.set_timestamp(WtDateTimeToTimestamp(date->date(), time->time()));

            cb(resultItem);
        }

        delete this;
      }));
   }
};

class MateriaCalendar : public Wt::WCalendar
{
public:
    MateriaCalendar(calendar::CalendarService_Stub& calendarService)
    : mCalendarService(calendarService)
    {
        populate();

        currentPageChanged().connect([=]{populate();refresh();});
    }

    std::vector<calendar::CalendarItem> getItems(const Wt::WDate& date)
    {
        std::vector<calendar::CalendarItem> result;

        int timestampFrom = WtDateToTimeStamp(date);
        int timestampTo = WtDateToTimeStamp(date.addDays(1)) - 1;

        for(auto x : mItems.items())
        {
            if(x.timestamp() >= timestampFrom && x.timestamp() <= timestampTo)
            {
                result.push_back(x);
            }
        }

        return result;
    }

    void addItem(const calendar::CalendarItem& item)
    {
        mItems.add_items()->CopyFrom(item);
    }

    void removeItem(const calendar::CalendarItem& item)
    {
       for(int i = 0; i < mItems.items_size(); ++i)
       {
          if(mItems.items(i).id().guid() == item.id().guid())
          {
             mItems.mutable_items()->DeleteSubrange(i, 1);

             hide();
             show();

             break;     
          }
       }
    }

    virtual ~MateriaCalendar(){}

private:
    void populate()
    {
        mItems.Clear();

        calendar::TimeRange timeRange;
        auto firstDay = boost::gregorian::date(currentYear(), static_cast<boost::date_time::months_of_year>(currentMonth()), 1);

        timeRange.set_timestampfrom(boost::posix_time:: to_time_t(boost::posix_time::ptime(firstDay - boost::gregorian::date_duration(6))));
        timeRange.set_timestampto(boost::posix_time:: to_time_t(boost::posix_time::ptime(firstDay + boost::gregorian::date_duration(37))));

        mCalendarService.Query(nullptr, &timeRange, &mItems, nullptr);

        printf("Population: %d", mItems.items_size());
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

    calendar::CalendarService_Stub& mCalendarService;
    calendar::CalendarItems mItems;
};

struct calendarByTimestampCmp
{
    bool operator()(const calendar::CalendarItem& lhs, const calendar::CalendarItem& rhs) 
    {
        return lhs.timestamp() < rhs.timestamp();
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

    void addItem(const calendar::CalendarItem& item)
    {
        auto w = createWidget(item);
        mItems.insert(std::make_pair(item, w));
        addWidget(std::unique_ptr<Wt::WWidget>(w));
    }

    bool removeItem(const calendar::CalendarItem& item)
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

    Wt::Signal<calendar::CalendarItem>& onItemCtrlClicked()
    {
       return mOnItemCtrlClicked;
    }

    Wt::Signal<calendar::CalendarItem>& onItemDblClicked()
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

   Wt::WWidget* createWidget(const calendar::CalendarItem& item)
   {
      std::string text;

      switch(mDisplayMode)
      {
         case DisplayMode::Time:
         {
               auto ptime = boost::posix_time::from_time_t(item.timestamp());

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
               auto itemTime = boost::posix_time::from_time_t(item.timestamp());

               auto diff = itemTime - curTime;

               text = boost::lexical_cast<std::string>(diff.hours() / 24);
         }
         break;
      }

      auto textWidget = new Wt::WText(text + " " + item.text() + "<br></br><br></br>");

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

   Wt::Signal<calendar::CalendarItem> mOnItemCtrlClicked;
   Wt::Signal<calendar::CalendarItem> mOnItemDblClicked;
   std::multimap<calendar::CalendarItem, Wt::WWidget*, calendarByTimestampCmp> mItems;
   const DisplayMode mDisplayMode;
};

class CalendarView : public Wt::WContainerWidget
{
public:
    CalendarView(calendar::CalendarService_Stub& service)
    : mService(service)
    {
        mCalendarWidget = new MateriaCalendar(mService);
        mCalendarWidget->clicked().connect([=]{onCalendarClicked();} );
        addWidget(std::unique_ptr<Wt::WCalendar>(mCalendarWidget));

        mDateCalendar = addWidget(std::unique_ptr<CalendarItemList>(new CalendarItemList("", CalendarItemList::DisplayMode::Time)));
        mDateCalendar->onItemCtrlClicked().connect([=](calendar::CalendarItem x){initiateItemDelete(x);} );
        mDateCalendar->onItemDblClicked().connect([=](calendar::CalendarItem x){initiateItemEdit(x);} );
        mNextCalendar = addWidget(std::unique_ptr<CalendarItemList>(new CalendarItemList("Upcoming", CalendarItemList::DisplayMode::Countdown)));
        mNextCalendar->onItemCtrlClicked().connect([=](calendar::CalendarItem x){initiateItemDelete(x);} );
        mNextCalendar->onItemDblClicked().connect([=](calendar::CalendarItem x){initiateItemEdit(x);} );

        updateNextCalendar();

        mDateCalendar->clicked().connect([=]( Wt::WMouseEvent x)
        {
            if(x.modifiers().test(Wt::KeyboardModifier::Shift) && !mCalendarWidget->selection().empty())
            {
                initiateItemAdd(*mCalendarWidget->selection().begin());
            }  
        });
    }

private:
   void initiateItemAdd(const Wt::WDate date)
   {
      calendar::CalendarItem item; 
      item.set_timestamp(WtDateTimeToTimestamp(date, Wt::WTime(9, 0)));
      CalendarItemDialog* dlg = new CalendarItemDialog(item, [=](const calendar::CalendarItem& result) 
      {
         common::UniqueId id;
         mService.AddItem(nullptr, &result, &id, nullptr);

         if(!id.guid().empty())
         {
            calendar::CalendarItem item;
            item.CopyFrom(result);
            item.mutable_id()->set_guid(id.guid());
            mCalendarWidget->addItem(item);

            mDateCalendar->setTitle(date.toString());
            mDateCalendar->addItem(item);
            mDateCalendar->updateItemsOrdering();

            if(item.timestamp() < mNextCalendarHighestItemTimestamp)
            {
               updateNextCalendar();    
            }
         }
      });

      dlg->show();
   }

   void initiateItemDelete(const calendar::CalendarItem item)
   {
      std::function<void()> elementDeletedFunc = [=] () {
         common::OperationResultMessage dummy;
         mService.DeleteItem(nullptr, &item.id(), &dummy, nullptr);

         mDateCalendar->removeItem(item);
         if(mNextCalendar->removeItem(item))
         {
            updateNextCalendar();
         }
         mCalendarWidget->removeItem(item);

         auto items = mCalendarWidget->getItems(timestampToWtDate(item.timestamp()));
         if(items.empty())
         {
            mDateCalendar->clearAll();
         }

         };

      CommonDialogManager::showConfirmationDialog("Delete it?", elementDeletedFunc);
   }

   void initiateItemEdit(const calendar::CalendarItem item)
   {
      const calendar::CalendarItem prevItem(item);
      CalendarItemDialog* dlg = new CalendarItemDialog(item, [=](const calendar::CalendarItem& result) 
      {
         common::OperationResultMessage opResult;
         mService.EditItem(nullptr, &result, &opResult, nullptr);

         if(opResult.success())
         {
            mCalendarWidget->removeItem(prevItem);
            mCalendarWidget->addItem(result);
            mCalendarWidget->refresh();

            mDateCalendar->removeItem(prevItem);
            Wt::WDate date = *mCalendarWidget->selection().begin();
            if(timestampToWtDate(result.timestamp()) == date)
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

   void onCalendarClicked()
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

   void updateNextCalendar()
   {
        mNextCalendar->clearAll();
        mNextCalendar->setTitle("Upcoming");

        calendar::NextQueryParameters query;
        query.set_timestampfrom(WtDateTimeToTimestamp(Wt::WDateTime::currentDateTime().date(), Wt::WDateTime::currentDateTime().time()));	
        query.set_limit(5);

        calendar::CalendarItems items;
        mService.Next(nullptr, &query, &items, nullptr);

        for(auto x : items.items())
        {
            mNextCalendar->addItem(x);
            mNextCalendarHighestItemTimestamp = x.timestamp();
        }
   }

   calendar::CalendarService_Stub& mService;
   MateriaCalendar* mCalendarWidget;
   CalendarItemList* mDateCalendar;
   CalendarItemList* mNextCalendar;

   int mNextCalendarHighestItemTimestamp = 0;
};

ActionsView::ActionsView()
{
   addStyleClass("container-fluid");

   mService.reset(new MateriaServiceProxy<actions::ActionsService>("WebApp"));
   mActions = &mService->getService();
   mCService.reset(new MateriaServiceProxy<calendar::CalendarService>("WebApp"));
   mCalendar = &mCService->getService();

   auto actionsGroup = new Wt::WGroupBox;
   actionsGroup->addStyleClass("col-md-10");
   auto calendarGroup = new Wt::WGroupBox;
   calendarGroup->addStyleClass("col-md-2");

   actionsGroup->addWidget(Wt::cpp14::make_unique< Wt::WText>("Ctrl+click = delete, shift+click = add    "));

   auto tree = new Wt::WTree();
   auto root = new ActionsRootTreeNode(*mActions);

   auto cbMoveMode = new Wt::WCheckBox("Move mode");
   cbMoveMode->checked().connect(std::bind([=](){root->setMoveMode(true);}));
   cbMoveMode->unChecked().connect(std::bind([=](){root->setMoveMode(false);}));
   actionsGroup->addWidget(std::unique_ptr<Wt::WCheckBox>(cbMoveMode));
   
   tree->setTreeRoot(std::unique_ptr<ActionsRootTreeNode>(root));
   tree->setStyleClass("custom-tree");
   root->setLoadPolicy(Wt::ContentLoading::Lazy);
   root->expand();

   calendarGroup->addWidget(std::unique_ptr<Wt::WWidget>(new CalendarView(*mCalendar)));
   actionsGroup->addWidget(std::unique_ptr<Wt::WTree>(tree));

   addWidget(std::unique_ptr<Wt::WGroupBox>(actionsGroup));
   addWidget(std::unique_ptr<Wt::WGroupBox>(calendarGroup));
}