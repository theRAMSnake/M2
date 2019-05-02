#pragma once

#include <boost/signals2/signal.hpp>
#include <Wt/WPaintedWidget.h>
#include <Wt/WPainter.h>
#include <Wt/WLabel.h>
#include <Wt/WText.h>
#include <Wt/WPanel.h>
#include <Wt/WLineEdit.h>
#include <Wt/WTextArea.h>
#include <Wt/WCheckBox.h>
#include "../dialog/CommonDialogManager.hpp"
#include "../TaskEditDialog.hpp"
#include "GraphView.hpp"
#include "GraphEditDialog.hpp"

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
         mGraphView = addWidget(std::make_unique<GraphView>());
         mGraphView->OnCaptionClicked.connect(std::bind(&GoalViewCtrl<isCompact>::onBoundClicked, this, std::placeholders::_1));

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

         mObjsPanel = frameObj->addWidget(std::make_unique<Wt::WPanel>());
         mObjsPanel->addStyleClass("GoalViewCtrl_Objectives");

         mObjs = new Wt::WContainerWidget();
         mObjsPanel->setCentralWidget(std::unique_ptr<Wt::WContainerWidget>(mObjs));
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
      mGoal = goal;
      mGraph = mModel.getGraph(mGoal->id);

      setStyleClass(GoalViewCtrlTraits<isCompact>::nonemptyStyleClass);

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
      mGraph.reset();

      if(!isCompact)
      {
         mGraphView->reset();
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
      if(mGoal)
      {
         if(event.modifiers().test(Wt::KeyboardModifier::Control))
         {
            std::function<void()> elementDeletedFunc = [=] () {
               mModel.deleteGoal(mGoal->id);
               detach();
            };

            CommonDialogManager::showConfirmationDialog("Delete it?", elementDeletedFunc);
         }
         else if(!mGraph)
         {
            GoalEditDialog::TOnOkCallback cb([&] (auto g) {doModifyGoal(g);});
            GoalEditDialog* dlg = new GoalEditDialog(*mGoal, cb);
            dlg->show();
         }
         else
         {
            GraphEditDialog* dlg = new GraphEditDialog(*mGoal, mModel, [&] (auto g) {mGraph = g; render();});
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
         if(!mGraph)
         {
            mName->show();
            mGraphView->reset();
            mTasks->clear();
            for(auto t : mModel.getGoalTasks(mGoal->id))
            {
               addTaskWidget(t);
            }

            mObjsPanel->show();
            mObjs->clear();
            for(auto o : mModel.getGoalObjectives(mGoal->id))
            {
               addObjectiveWidget(o);
            }

            mNotes->setText(mGoal->notes);
         }
         else
         {
            mGraphView->assign(*mGraph, mGoal->title);
            mObjsPanel->hide();
            mName->hide();
         }
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
   Wt::WPanel* mObjsPanel = nullptr;
   GraphView* mGraphView = nullptr;
   std::optional<StrategyModel::Goal> mGoal;
   std::optional<StrategyModel::Graph> mGraph;
   bool mIsActiveSlot;
};