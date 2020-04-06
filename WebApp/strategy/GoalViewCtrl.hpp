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

      finished().connect(std::bind([=, this]() {
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

class IGoalViewCtrl
{
public:
   boost::signals2::signal<void (const materia::Id)> onRefreshOtherGoalRequest;

   virtual bool isEmpty() const = 0;
   virtual void attach(const StrategyModel::Goal& goal) = 0;
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
   FreeDataModel& freeData;
   ChallengeModel& chModel;
   bool isActiveSlot;
};

template<bool isCompact>
class GoalViewCtrl : public Wt::WContainerWidget, public IGoalViewCtrl
{
public:
   GoalViewCtrl(GoalViewCtrlConstructionParams& p)
   : mModel(p.model)
   , mFreeData(p.freeData)
   , mChModel(p.chModel)
   , mIsActiveSlot(p.isActiveSlot)
   {
      setStyleClass(GoalViewCtrlTraits<isCompact>::emptyStyleClass);
      addStyleClass("row");

      if(!isCompact)
      {
         mGraphView = addWidget(std::make_unique<GraphView>(mModel, mFreeData, mChModel));
         mGraphView->OnCaptionClicked.connect(std::bind(&GoalViewCtrl<isCompact>::onBoundClicked, this, std::placeholders::_1));
      }
      else
      {
         auto namePtr = std::make_unique<Wt::WLabel>("");
         mName = namePtr.get();
         mName->setStyleClass("GoalViewCtrl_Text");
         setAttributeValue("oncontextmenu", "event.cancelBubble = true; event.returnValue = false; return false;");
         mName->mouseWentDown().connect(this, &GoalViewCtrl<isCompact>::onBoundClicked);
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

   std::optional<StrategyModel::Goal> detach() override
   {
      auto result = mGoal;

      setStyleClass(GoalViewCtrlTraits<isCompact>::emptyStyleClass);
      
      mGoal.reset();
      mGraph.reset();

      if(!isCompact)
      {
         mGraphView->reset();
      }
      else
      {
         mName->setText("");
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
            std::function<void()> elementDeletedFunc = [this] () {
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
            GraphEditDialog* dlg = new GraphEditDialog(*mGoal, mModel, mFreeData, mChModel,
               [&] (auto graph, auto goal) {mGraph = graph; mGoal = goal; render();});
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
      if(!isCompact)
      {
         if(!mGraph)
         {
            mGraphView->reset();
         }
         else
         {
            mGraphView->assign(mGoal->id, *mGraph, mGoal->title);
         }
      }
      else
      {
         mName->setText(mGoal->title);
      }
   }


   static constexpr char MY_MIME_TYPE[] = "GoalViewCtrl";
   StrategyModel& mModel;
   FreeDataModel& mFreeData;
   ChallengeModel& mChModel;
   Wt::WLabel* mName = nullptr;
   GraphView* mGraphView = nullptr;
   std::optional<StrategyModel::Goal> mGoal;
   std::optional<StrategyModel::Graph> mGraph;
   bool mIsActiveSlot;
};