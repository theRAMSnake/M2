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

class GoalEditDialog : public BasicDialog
{
public:
   typedef std::function<void(const StrategyModel::Goal&)> TOnOkCallback;
   GoalEditDialog(const StrategyModel::Goal& subject, TOnOkCallback cb)
   : BasicDialog("Goal Edit")
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

//--------------------------------------------------------------------------------------

class IGoalViewCtrl
{
public:
   virtual bool isEmpty() const = 0;
   virtual void attach(const StrategyModel::Goal& goal) = 0;
   virtual void addTask(const StrategyModel::Task& task) = 0;
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
};

template<>
class GoalViewCtrlTraits<false>
{
public:
   static constexpr const char* emptyStyleClass = "GoalViewCtrlEmpty";
   static constexpr const char* nonemptyStyleClass = "GoalViewCtrl";
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
   }

private:
   StrategyModel::Task mTask;
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

      mName = addWidget(std::make_unique<Wt::WLabel>(""));
      mName->setStyleClass("GoalViewCtrl_Text");
      setAttributeValue("oncontextmenu", "event.cancelBubble = true; event.returnValue = false; return false;");
      mName->mouseWentDown().connect(this, &GoalViewCtrl<isCompact>::onBoundClicked);

      if(!isCompact)
      {
         addWidget(std::make_unique<Wt::WLabel>("<br></br>"));

         mTasks = new Wt::WContainerWidget();
         addWidget(std::unique_ptr<Wt::WContainerWidget>(mTasks));

         mNotes = addWidget(std::make_unique<Wt::WLabel>(""));
         mNotes->setStyleClass("GoalViewCtrl_Notes");
      }

      acceptDrops(MY_MIME_TYPE);
      setDraggable(MY_MIME_TYPE);
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

   std::optional<StrategyModel::Goal> detach() override
   {
      auto result = mGoal;

      setStyleClass(GoalViewCtrlTraits<isCompact>::emptyStyleClass);
      mName->setText("");
      mTasks->clear();
      mGoal.reset();
      if(!isCompact)
      {
         mNotes->setText("");
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

         mNotes->setText(mGoal->notes);
      }
   }

   void addTaskWidget(const StrategyModel::Task& t)
   {
      auto w = mTasks->addWidget(std::make_unique<TaskWidget>(t));
      auto b = std::bind(&GoalViewCtrl<isCompact>::onTaskWidgetClicked, this, w, t, std::placeholders::_1);

      w->mouseWentDown().connect(b);
   }

   void onTaskWidgetClicked(TaskWidget* w, const StrategyModel::Task t, Wt::WMouseEvent event)
   {
      if(event.button() == Wt::MouseButton::Right)
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
               std::bind(&GoalViewCtrl<isCompact>::onTaskEditDialogOk, this, std::placeholders::_1, std::placeholders::_2, t, w));
            dlg->show();
         }
      }
   }

   void onTaskEditDialogOk(const std::string& title, const std::string& notes, const StrategyModel::Task src, TaskWidget* w)
   {
      auto newTask = src;

      newTask.title = title;
      newTask.notes = notes;

      w->setTask(newTask);

      mModel.modifyTask(newTask);
   }

   static constexpr char MY_MIME_TYPE[] = "GoalViewCtrl";
   StrategyModel& mModel;
   Wt::WLabel* mName = nullptr;
   Wt::WLabel* mNotes = nullptr;
   Wt::WContainerWidget* mTasks = nullptr;
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

   addWidget(std::unique_ptr<Wt::WWidget>(temp));

   layGoals();
   fillResources(*mMainToolbar);
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

void StrategyView::onAddTaskClick()
{
   std::vector<std::string> choises;
   for(auto g : mModel.getGoals())
   {
      choises.push_back(g.title);
   }

   CommonDialogManager::showChoiseDialog(choises, [=](auto selected) {
      
      std::function<void(std::string)> nextFunc = [=](std::string title){
         auto item = mModel.addTask(title, mModel.getGoals()[selected].id);
         
         for(auto gc : mGoalCtrls)
         {
            if(!gc->isEmpty() && gc->getGoal().id == mModel.getGoals()[selected].id)
            {
               gc->addTask(item);
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

   dialog->setWidth(Wt::WLength("85%"));
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