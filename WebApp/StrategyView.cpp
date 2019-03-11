#include "StrategyView.hpp"
#include "CommonDialogManager.hpp"
#include "TemplateBuilder.hpp"
#include "TaskEditDialog.hpp"
#include "Wt/WToolBar.h"
#include "Wt/WPushButton.h"
#include "Wt/WLabel.h"
#include "Wt/WTable.h"
#include "Wt/WCssDecorationStyle.h"
#include "Wt/WCheckBox.h"
#include "Wt/WGroupBox.h"
#include "Wt/WLineEdit.h"
#include "Wt/WTextArea.h"
#include "Wt/WPaintedWidget.h"
#include "Wt/WPainter.h"
#include "Wt/WPopupMenu.h"
#include "Wt/WDoubleSpinBox.h"
#include "Wt/WPanel.h"
#include "Wt/WComboBox.h"
#include <boost/signals2/signal.hpp>

class GoalEditDialog : public BasicDialog
{
public:
   typedef std::function<void(const StrategyModel::Goal&)> TOnOkCallback;
   GoalEditDialog(const StrategyModel::Goal& subject, TOnOkCallback cb)
   : BasicDialog("Goal Edit", true)
   {
      mTitle = new Wt::WLineEdit(subject.title);
      contents()->addWidget(std::unique_ptr<Wt::WLineEdit>(mTitle));

      mNotes = new Wt::WTextArea();
      mNotes->setHeight(500);
      mNotes->setText(subject.notes);
      mNotes->setMargin("5px", Wt::Side::Top);
      
      contents()->addWidget(std::unique_ptr<Wt::WTextArea>(mNotes));

      finished().connect(std::bind([=]() {
        if (result() == Wt::DialogCode::Accepted)
        {
           StrategyModel::Goal newGoal = subject;
           newGoal.title = mTitle->text().narrow();
           newGoal.notes = mNotes->text().narrow();
           cb(newGoal);
        }

        delete this;
      }));
   }

private:
   Wt::WLineEdit* mTitle;
   Wt::WTextArea* mNotes;
};

class ResourceEditDialog : public BasicDialog
{
public:
   typedef std::function<void(const StrategyModel::Resource&)> TOnOkCallback;
   ResourceEditDialog(const StrategyModel::Resource& subject, TOnOkCallback cb)
   : BasicDialog("Resource Edit")
   {
      mTitle = new Wt::WLineEdit(subject.name);
      contents()->addWidget(std::unique_ptr<Wt::WLineEdit>(mTitle));

      mValue = new Wt::WDoubleSpinBox();
      contents()->addWidget(std::unique_ptr<Wt::WDoubleSpinBox>(mValue));

      mValue->setRange(-1000000,1000000);
      mValue->setValue(subject.value);
      mValue->setDecimals(0);
      mValue->setSingleStep(1);
      
      finished().connect(std::bind([=]() {
        if (result() == Wt::DialogCode::Accepted)
        {
           StrategyModel::Resource newResource = subject;
           newResource.name = mTitle->text().narrow();
           newResource.value = mValue->value();
           cb(newResource);
        }

        delete this;
      }));
   }

private:
   Wt::WLineEdit* mTitle;
   Wt::WDoubleSpinBox* mValue;
};

class ObjectiveEditDialog : public BasicDialog
{
public:
   typedef std::function<void(const StrategyModel::Objective&)> TOnOkCallback;
   ObjectiveEditDialog(const StrategyModel::Objective& subject, const std::vector<StrategyModel::Resource>& resources, TOnOkCallback cb)
   : BasicDialog("Objective Edit")
   {
      mTitle = new Wt::WLineEdit(subject.title);
      contents()->addWidget(std::unique_ptr<Wt::WLineEdit>(mTitle));

      mReached = new Wt::WCheckBox("Reached");
      mReached->setInline(false);
      mReached->setChecked(subject.reached);
      contents()->addWidget(std::unique_ptr<Wt::WCheckBox>(mReached));

      mResources = new Wt::WComboBox();
      mResources->setInline(false);
      mResources->addItem("None");
      mResourceRequirement = new Wt::WLineEdit();
      for(auto x : resources)
      {
         mResources->addItem(x.name.c_str());
         if(subject.resId == x.id)
         {
            mResources->setCurrentIndex(mResources->count() - 1);
            mResourceRequirement->setText(std::to_string(subject.expectedResValue));
         }
      }
      contents()->addWidget(std::unique_ptr<Wt::WComboBox>(mResources));
      contents()->addWidget(std::unique_ptr<Wt::WLineEdit>(mResourceRequirement));
      
      finished().connect(std::bind([=]() {
        if (result() == Wt::DialogCode::Accepted)
        {
            StrategyModel::Objective newObjective = subject;
            newObjective.title = mTitle->text().narrow();
            newObjective.reached = mReached->isChecked();
           
            auto pos = std::find_if(resources.begin(), resources.end(), [=](auto x){
               return x.name == mResources->currentText().narrow();
            }); 

            if(pos != resources.end())
            {
               newObjective.resId = pos->id;
               newObjective.expectedResValue = std::stoi(mResourceRequirement->text().narrow());
            }

            else
            {
               newObjective.resId = materia::Id::Invalid;
               newObjective.expectedResValue = 0;
            }
           
            cb(newObjective);
        }

        delete this;
      }));
   }

private:
   Wt::WComboBox* mResources;
   Wt::WCheckBox* mReached;
   Wt::WLineEdit* mTitle;
   Wt::WLineEdit* mResourceRequirement;
};

//--------------------------------------------------------------------------------------

class IGoalViewCtrl
{
public:
   boost::signals2::signal<void (const materia::Id)> onRefreshOtherGoalRequest;

   virtual bool isEmpty() const = 0;
   virtual void attach(const StrategyModel::Goal& goal) = 0;
   virtual void addTask(const StrategyModel::Task& task) = 0;
   virtual void addObjective(const StrategyModel::Objective& o) = 0;
   virtual std::optional<StrategyModel::Goal> detach() = 0;
   virtual const StrategyModel::Goal& getGoal() const = 0;

   virtual ~IGoalViewCtrl() {}
};

template<bool isCompact>
class GoalViewCtrlTraits
{
public:

};

template<>
class GoalViewCtrlTraits<true>
{
public:
   static constexpr const char* emptyStyleClass = "GoalViewCtrlSmallEmpty";
   static constexpr const char* nonemptyStyleClass = "GoalViewCtrlSmall";
   static constexpr bool isMovable = true;
};

template<>
class GoalViewCtrlTraits<false>
{
public:
   static constexpr const char* emptyStyleClass = "GoalViewCtrlEmpty";
   static constexpr const char* nonemptyStyleClass = "GoalViewCtrl";
   static constexpr bool isMovable = false;
};

struct GoalViewCtrlConstructionParams
{
   StrategyModel& model;
   bool isActiveSlot;
};

class TaskWidget : public Wt::WPaintedWidget
{
public:
   TaskWidget(const StrategyModel::Task& t)
   : mTask(t)
   {
      resize(32, 32);
      setInline(true);
      setMargin(5, Wt::Side::Left);
      setToolTip(t.title);
   }

   void setTask(const StrategyModel::Task& t)
   {
      mTask = t;
      setToolTip(t.title);
   }

protected:
   void paintEvent(Wt::WPaintDevice *paintDevice) 
   {
      Wt::WPainter painter(paintDevice);
      painter.setBrush(Wt::WBrush(Wt::WColor(Wt::StandardColor::Magenta)));
      painter.drawRect(0, 0, 32, 32);
      if(!mTask.title.empty())
      {
         painter.drawText(Wt::WRectF(0, 0, 32, 32), Wt::AlignmentFlag::Middle | Wt::AlignmentFlag::Center, mTask.title.substr(0, 1));
      }
   }

private:
   StrategyModel::Task mTask;
};

class ObjectiveWidget : public Wt::WLabel
{
public:
   ObjectiveWidget(const StrategyModel::Objective& o)
   : mObj(o)
   {
      setInline(false);
      setTextFormat(Wt::TextFormat::XHTML);

      setObjective(o);
   }

   void setObjective(const StrategyModel::Objective& o)
   {
      mObj = o;
      
      auto t = o.title;
      if(o.reached)
      {
         t = "<s>" + t + "</s>";
      }

      setText(t);
   }

   const StrategyModel::Objective& getObjective() const
   {
      return mObj;
   }

private:
   StrategyModel::Objective mObj;
};

template<bool isCompact>
class GoalViewCtrl : public Wt::WContainerWidget, public IGoalViewCtrl
{
public:
   GoalViewCtrl(GoalViewCtrlConstructionParams& p)
   : mModel(p.model)
   , mIsActiveSlot(p.isActiveSlot)
   {
      setStyleClass(GoalViewCtrlTraits<isCompact>::emptyStyleClass);
      addStyleClass("row");

      auto namePtr = std::make_unique<Wt::WLabel>("");
      mName = namePtr.get();
      mName->setStyleClass("GoalViewCtrl_Text");
      setAttributeValue("oncontextmenu", "event.cancelBubble = true; event.returnValue = false; return false;");
      mName->mouseWentDown().connect(this, &GoalViewCtrl<isCompact>::onBoundClicked);

      if(!isCompact)
      {
         WContainerWidget* frame = addWidget(std::make_unique<Wt::WContainerWidget>());
         frame->setStyleClass("col-md-6");
         frame->addWidget(std::make_unique<Wt::WLabel>("<br></br>"));

         frame->addWidget(std::move(namePtr));

         mTasks = new Wt::WContainerWidget();
         frame->addWidget(std::unique_ptr<Wt::WContainerWidget>(mTasks));

         mNotes = frame->addWidget(std::make_unique<Wt::WText>(""));
         mNotes->setStyleClass("GoalViewCtrl_Notes");
         mNotes->setTextFormat(Wt::TextFormat::Plain);

         WContainerWidget* frameObj = addWidget(std::make_unique<Wt::WContainerWidget>());
         frameObj->setStyleClass("col-md-6");

         auto panel = frameObj->addWidget(std::make_unique<Wt::WPanel>());
         panel->addStyleClass("GoalViewCtrl_Objectives");

         mObjs = new Wt::WContainerWidget();
         panel->setCentralWidget(std::unique_ptr<Wt::WContainerWidget>(mObjs));
      }
      else
      {
         addWidget(std::move(namePtr));
      }

      if(GoalViewCtrlTraits<isCompact>::isMovable)
      {
         acceptDrops(MY_MIME_TYPE);
         setDraggable(MY_MIME_TYPE);
      }
   }

   void dropEvent(Wt::WDropEvent dropEvent) override
   {
      auto other = dynamic_cast<GoalViewCtrl<isCompact>*>(dropEvent.source());

      if(other == this)
      {
         return;
      }

      auto otherGoal = other->detach();
      auto myGoal = detach();

      if(otherGoal)
      {
         attach(*otherGoal);
      }
      if(myGoal)
      {
         other->attach(*myGoal);
      }
   }

   bool isEmpty() const override
   {
      return !mGoal;
   }

   const StrategyModel::Goal& getGoal() const override
   {
      return *mGoal;
   }

   void attach(const StrategyModel::Goal& goal) override
   {
      setStyleClass(GoalViewCtrlTraits<isCompact>::nonemptyStyleClass);

      mGoal = goal;

      render();

      if(mIsActiveSlot != mGoal->focused)
      {
         mGoal->focused = mIsActiveSlot;
         doModifyGoal(*mGoal);
      }
   }

   void addTask(const StrategyModel::Task& task) override
   {
      if(mGoal)
      {
         addTaskWidget(task);
      }
   }

   void addObjective(const StrategyModel::Objective& o) override
   {
      if(mGoal)
      {
         addObjectiveWidget(o);
      }
   }

   std::optional<StrategyModel::Goal> detach() override
   {
      auto result = mGoal;

      setStyleClass(GoalViewCtrlTraits<isCompact>::emptyStyleClass);
      mName->setText("");
      mGoal.reset();

      if(!isCompact)
      {
         mNotes->setText("");
         mTasks->clear();
         mObjs->clear();
      }

      return result;
   }

   const std::string getMimeType() const
   {
      return MY_MIME_TYPE;
   }

private:
   void onBoundClicked(Wt::WMouseEvent event)
   {
      if(event.button() == Wt::MouseButton::Right && mGoal)
      {
         if(event.modifiers().test(Wt::KeyboardModifier::Control))
         {
            std::function<void()> elementDeletedFunc = [=] () {
               mModel.deleteGoal(mGoal->id);
               detach();
            };

            CommonDialogManager::showConfirmationDialog("Delete it?", elementDeletedFunc);
         }
         else
         {
            GoalEditDialog::TOnOkCallback cb([&] (auto g) {doModifyGoal(g);});
            GoalEditDialog* dlg = new GoalEditDialog(*mGoal, cb);
            dlg->show();
         }
      }
   }

   void doModifyGoal(const StrategyModel::Goal& newGoal)
   {
      mGoal = newGoal;
      render();

      mModel.modifyGoal(newGoal);
   }

   void render()
   {
      mName->setText(mGoal->title);
      if(!isCompact)
      {
         mTasks->clear();
         for(auto t : mModel.getGoalTasks(mGoal->id))
         {
            addTaskWidget(t);
         }

         mObjs->clear();
         for(auto o : mModel.getGoalObjectives(mGoal->id))
         {
            addObjectiveWidget(o);
         }

         mNotes->setText(mGoal->notes);
      }
   }

   void addTaskWidget(const StrategyModel::Task& t)
   {
      auto w = mTasks->addWidget(std::make_unique<TaskWidget>(t));
      auto b = std::bind(&GoalViewCtrl<isCompact>::onTaskWidgetClicked, this, w, t, std::placeholders::_1);

      w->mouseWentDown().connect(b);
   }

   void addObjectiveWidget(const StrategyModel::Objective& o)
   {
      auto w = mObjs->addWidget(std::make_unique<ObjectiveWidget>(o));
      auto b = std::bind(&GoalViewCtrl<isCompact>::onObjectiveWidgetClicked, this, w, std::placeholders::_1);
      w->mouseWentDown().connect(b);
   }

   void onTaskWidgetClicked(TaskWidget* w, const StrategyModel::Task t, Wt::WMouseEvent event)
   {
      if(event.button() == Wt::MouseButton::Left)
      {
         if(event.modifiers().test(Wt::KeyboardModifier::Control))
         {
            std::function<void()> elementDeletedFunc = [=] () {
               mModel.deleteTask(t.id);
               mTasks->removeChild(w);
            };

            CommonDialogManager::showConfirmationDialog("Delete it?", elementDeletedFunc);
         }
         else
         {
            auto dlg = new TaskEditDialog(
               t.title,
               t.notes,
               t.parentGoalId,
               mModel.getGoals(),
               std::bind(&GoalViewCtrl<isCompact>::onTaskEditDialogOk, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, t, w));
            dlg->show();
         }
      }
   }

   void onObjectiveWidgetClicked(ObjectiveWidget* w, Wt::WMouseEvent event)
   {
      if(event.button() == Wt::MouseButton::Left)
      {
         if(event.modifiers().test(Wt::KeyboardModifier::Control))
         {
            std::function<void()> elementDeletedFunc = [=] () {
               mModel.deleteObjective(w->getObjective().id);
               mObjs->removeChild(w);
            };

            CommonDialogManager::showConfirmationDialog("Delete it?", elementDeletedFunc);
         }
         else
         {
            auto dlg = new ObjectiveEditDialog(
               w->getObjective(),
               mModel.getResources(),
               std::bind(&GoalViewCtrl<isCompact>::onObjectiveEditDialogOk, this, std::placeholders::_1, w));
            dlg->show();
         }
      }
   }

   void onTaskEditDialogOk(const std::string& title, const std::string& notes, const materia::Id& goalId, const StrategyModel::Task src, TaskWidget* w)
   {
      auto newTask = src;

      newTask.title = title;
      newTask.notes = notes;
      newTask.parentGoalId = goalId;

      w->setTask(newTask);

      mModel.modifyTask(newTask);

      if(newTask.parentGoalId != mGoal->id)
      {
         mTasks->removeChild(w);
         onRefreshOtherGoalRequest(newTask.parentGoalId);
      }
   }

   void onObjectiveEditDialogOk(const StrategyModel::Objective src, ObjectiveWidget* w)
   {
      auto newObj = src;

      w->setObjective(mModel.modifyObjective(newObj));
   }

   static constexpr char MY_MIME_TYPE[] = "GoalViewCtrl";
   StrategyModel& mModel;
   Wt::WLabel* mName = nullptr;
   Wt::WText* mNotes = nullptr;
   Wt::WContainerWidget* mTasks = nullptr;
   Wt::WContainerWidget* mObjs = nullptr;
   std::optional<StrategyModel::Goal> mGoal;
   bool mIsActiveSlot;
};

//------------------------------------------------------------------------------------------------------------

class BacklogView : public Wt::WContainerWidget
{
public:
   BacklogView(StrategyModel& model)
   : mModel(model)
   {
      auto activeGroup = addWidget(std::make_unique<Wt::WGroupBox>("Active"));
      auto [temp, activeGoalCtrls] = TemplateBuilder::makeTable<GoalViewCtrl<true>>(1u, 6u, GoalViewCtrlConstructionParams{model, true});
      activeGroup->addWidget(std::unique_ptr<Wt::WTemplate>(temp));

      auto items = mModel.getGoals();

      int numUnfocused = 6;
      std::size_t pos = 0;
      for(auto g : items)
      {
         if(g.focused)
         {
            if(pos < activeGoalCtrls.size())
            {
               activeGoalCtrls[pos++]->attach(g);
            }
         }
         else
         {
            numUnfocused++;
         }
      }

      auto inactiveGroup = addWidget(std::make_unique<Wt::WGroupBox>("Inactive"));
      auto [temp2, inactiveGoalCtrls] = TemplateBuilder::makeTable<GoalViewCtrl<true>>(numUnfocused / 6 + 1, 6u, GoalViewCtrlConstructionParams{model, false});
      inactiveGroup->addWidget(std::unique_ptr<Wt::WTemplate>(temp2));

      pos = 0;
      for(auto g : items)
      {
         if(!g.focused)
         {
            if(pos < inactiveGoalCtrls.size())
            {
               inactiveGoalCtrls[pos++]->attach(g);
            }
         }
      }
   }

private:

   StrategyModel& mModel;
};

class ResourceViewCtrl : public Wt::WContainerWidget
{
public:
   ResourceViewCtrl(const StrategyModel::Resource& r, StrategyModel& model)
   : mModel(model)
   , mResource(r)
   {
      mName = addWidget(std::make_unique<Wt::WLabel>(r.name));
      mName->setStyleClass("ResourceViewCtrlText");
      auto valueText = std::to_string(r.value);
      mValue = addWidget(std::make_unique<Wt::WLabel>(valueText));
      mValue->setStyleClass("ResourceViewCtrlValue");

      clicked().connect(this, &ResourceViewCtrl::onClicked);
   }

   void updateStyle()
   {
      setStyleClass("ResourceViewCtrl");
   }

private:
   void onClicked(Wt::WMouseEvent event)
   {
      if(event.button() == Wt::MouseButton::Left)
      {
         if(event.modifiers().test(Wt::KeyboardModifier::Control))
         {
            std::function<void()> elementDeletedFunc = [=] () {
               mModel.deleteResource(mResource.id);
               parent()->removeChild(this);
            };

            CommonDialogManager::showConfirmationDialog("Delete it?", elementDeletedFunc);
         }
         else
         {
            auto dlg = new ResourceEditDialog(
               mResource,
               std::bind(&ResourceViewCtrl::onEditDialogOk, this, std::placeholders::_1));
            dlg->show();
         }
      }
   }

   void onEditDialogOk(const StrategyModel::Resource& newResource)
   {
      mName->setText(newResource.name);
      auto valueText = std::to_string(newResource.value);

      mValue->setText(valueText);

      mModel.modifyResource(newResource);

      mResource = newResource;
   }

   Wt::WLabel* mName;
   Wt::WLabel* mValue;
   StrategyModel& mModel;
   StrategyModel::Resource mResource;
};

//------------------------------------------------------------------------------------------------------------

StrategyView::StrategyView(StrategyModel& strategy)
: mModel(strategy)
{
   setMargin(5);

   mMainToolbar = addWidget(std::make_unique<Wt::WToolBar>());

   auto popupPtr = std::make_unique<Wt::WPopupMenu>();
   popupPtr->addItem("Goal")->triggered().connect(std::bind(&StrategyView::onAddGoalClick, this));
   popupPtr->addItem("Task")->triggered().connect(std::bind(&StrategyView::onAddTaskClick, this));
   popupPtr->addItem("Resource")->triggered().connect(std::bind(&StrategyView::onAddResourceClick, this));
   popupPtr->addItem("Objective")->triggered().connect(std::bind(&StrategyView::onAddObjectiveClick, this));

   auto addBtn = std::make_unique<Wt::WPushButton>("Add");
   addBtn->setStyleClass("btn-primary");
   addBtn->setMenu(std::move(popupPtr));
   mMainToolbar->addButton(std::move(addBtn));

   auto backlogBtn = std::make_unique<Wt::WPushButton>("Backlog");
   backlogBtn->setStyleClass("btn-primary");
   backlogBtn->clicked().connect(std::bind(&StrategyView::onBacklogClick, this));
   mMainToolbar->addButton(std::move(backlogBtn));

   auto [temp, goalCtrls] = TemplateBuilder::makeTable<GoalViewCtrl<false>>(3u, 2u, GoalViewCtrlConstructionParams{strategy, true});
   std::copy(goalCtrls.begin(), goalCtrls.end(), std::inserter(mGoalCtrls, mGoalCtrls.begin()));

   for(auto x : mGoalCtrls)
   {
      x->onRefreshOtherGoalRequest.connect(std::bind(&StrategyView::refreshGoalCtrl, this, std::placeholders::_1));
   }

   addWidget(std::unique_ptr<Wt::WWidget>(temp));

   layGoals();
   fillResources(*mMainToolbar);
}

void StrategyView::refreshGoalCtrl(const materia::Id& id)
{
   for(auto x : mGoalCtrls)
   {
      if(!x->isEmpty() && x->getGoal().id == id)
      {
         x->attach(*x->detach());
      }
   }
}

void StrategyView::fillResources(Wt::WToolBar& toolbar)
{
   toolbar.addSeparator();

   for(auto r : mModel.getResources())
   {
      toolbar.addWidget(std::make_unique<ResourceViewCtrl>(r, mModel), Wt::AlignmentFlag::Right);
   }
}

void StrategyView::onAddGoalClick()
{
   bool hasSlots = std::find_if(mGoalCtrls.begin(), mGoalCtrls.end(), [&](auto c)->bool {return c->isEmpty();})
      != mGoalCtrls.end();

   std::vector<std::string> choise = {"Backlog"};
   if(hasSlots)
   {
      choise.push_back("Active");
   }

   CommonDialogManager::showChoiseDialog(choise, [=](auto selected) {
      const bool isActive = selected == 1;

      std::function<void(std::string)> nextFunc = [this, isActive](std::string title){
         auto item = mModel.addGoal(isActive, title);
         if(isActive)
         {
            putGoal(item);
         }
      };

      CommonDialogManager::showOneLineDialog("Please specify title", "Title", "", nextFunc);
   });
}

void StrategyView::onAddResourceClick()
{     
   std::function<void(std::string)> nextFunc = [=](std::string name){
      auto item = mModel.addResource(name);
      
      mMainToolbar->addWidget(std::make_unique<ResourceViewCtrl>(item, mModel), Wt::AlignmentFlag::Right);
   };

   CommonDialogManager::showOneLineDialog("Please specify name", "Name", "", nextFunc);
}

inline bool goalsSorter(const StrategyModel::Goal &a, const StrategyModel::Goal &b)
{
   if(a.focused == b.focused)
   {
      return a.title < b.title;
   }

   return a.focused > b.focused;
}

void StrategyView::onAddTaskClick()
{
   auto goals = mModel.getGoals();
   std::sort(goals.begin(), goals.end(), goalsSorter);

   std::vector<std::string> choises;
   for(auto g : goals)
   {
      choises.push_back(g.title);
   }

   CommonDialogManager::showChoiseDialog(choises, [=](auto selected) {
      
      std::function<void(std::string)> nextFunc = [=](std::string title){
         auto item = mModel.addTask(title, goals[selected].id);
         
         for(auto gc : mGoalCtrls)
         {
            if(!gc->isEmpty() && gc->getGoal().id == goals[selected].id)
            {
               gc->addTask(item);
               break;
            }
         }
      };

      CommonDialogManager::showOneLineDialog("Please specify title", "Title", "", nextFunc);
   });
}

void StrategyView::onAddObjectiveClick()
{
   auto goals = mModel.getGoals();
   std::sort(goals.begin(), goals.end(), goalsSorter);

   std::vector<std::string> choises;
   for(auto g : goals)
   {
      choises.push_back(g.title);
   }

   CommonDialogManager::showChoiseDialog(choises, [=](auto selected) {
      
      std::function<void(std::string)> nextFunc = [=](std::string title){
         auto item = mModel.addObjective(title, goals[selected].id);
         
         for(auto gc : mGoalCtrls)
         {
            if(!gc->isEmpty() && gc->getGoal().id == goals[selected].id)
            {
               gc->addObjective(item);
               break;
            }
         }
      };

      CommonDialogManager::showOneLineDialog("Please specify title", "Title", "", nextFunc);
   });
}

void StrategyView::putGoal(const StrategyModel::Goal& goal)
{
   for(auto x : mGoalCtrls)
   {
      if(x->isEmpty())
      {
         x->attach(goal);
         break;
      }
   }
}

void StrategyView::onBacklogClick()
{
   Wt::WDialog* dialog = new Wt::WDialog("Backlog View");

   dialog->contents()->addWidget(std::unique_ptr<Wt::WWidget>(new BacklogView(mModel)));
   dialog->contents()->setOverflow(Wt::Overflow::Auto);

   Wt::WPushButton *ok = new Wt::WPushButton("OK");
   dialog->footer()->addWidget(std::unique_ptr<Wt::WPushButton>(ok));
   ok->setDefault(true);

   ok->clicked().connect(std::bind([=]() {
      dialog->accept();
   }));

   dialog->finished().connect(std::bind([=]() {
         layGoals();
         delete dialog;
   }));

   dialog->setWidth(Wt::WLength("95%"));
   dialog->setHeight(Wt::WLength("85%"));
   dialog->show();
}

void StrategyView::layGoals()
{
   for(auto c : mGoalCtrls)
   {
      c->detach();
   }

   for(auto g : mModel.getGoals())
   {
      if(g.focused)
      {
         putGoal(g);
      }
   }
}