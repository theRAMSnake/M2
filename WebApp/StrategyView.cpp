#include "StrategyView.hpp"
#include "CommonDialogManager.hpp"
#include "TemplateBuilder.hpp"
#include "Wt/WToolBar.h"
#include "Wt/WPushButton.h"
#include "Wt/WLabel.h"
#include "Wt/WTable.h"
#include "Wt/WCssDecorationStyle.h"
#include "Wt/WCheckBox.h"
#include "Wt/WGroupBox.h"
#include "Wt/WLineEdit.h"
#include "Wt/WTextArea.h"

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

//--------------------------------------------------------------------------------------

class IGoalViewCtrl
{
public:
   virtual bool isEmpty() const = 0;
   virtual void attach(const StrategyModel::Goal& goal) = 0;
   virtual void detach() = 0;

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
   using titleControl = Wt::WLabel;

   static void setTitle(const std::string& title, titleControl& ctrl)
   {
      ctrl.setText(title);
   }

   static Wt::WLabel* createNotes(titleControl& ctrl)
   {
      throw -1;
   }
};

template<>
class GoalViewCtrlTraits<false>
{
public:
   static constexpr const char* emptyStyleClass = "GoalViewCtrlEmpty";
   static constexpr const char* nonemptyStyleClass = "GoalViewCtrl";
   using titleControl = Wt::WGroupBox;

   static void setTitle(const std::string& title, titleControl& ctrl)
   {
      ctrl.setTitle(title);
   }

   static Wt::WLabel* createNotes(titleControl& ctrl)
   {
      return ctrl.addWidget(std::make_unique<Wt::WLabel>(""));
   }
};

template<bool isCompact>
class GoalViewCtrl : public Wt::WContainerWidget, public IGoalViewCtrl
{
public:
   GoalViewCtrl(StrategyModel& model)
   : mModel(model)
   {
      setStyleClass(GoalViewCtrlTraits<isCompact>::emptyStyleClass);

      mBound = addWidget(std::make_unique<typename GoalViewCtrlTraits<isCompact>::titleControl>(""));
      mBound->setStyleClass("GoalViewCtrl_Text");
      mBound->doubleClicked().connect(this, &GoalViewCtrl<isCompact>::onBoundDoubleClicked);

      if(!isCompact)
      {
         mNotes = GoalViewCtrlTraits<isCompact>::createNotes(*mBound);
         mNotes->setStyleClass("GoalViewCtrl_Notes");
      }
   }

   bool isEmpty() const override
   {
      return !mGoal;
   }

   void attach(const StrategyModel::Goal& goal) override
   {
      setStyleClass(GoalViewCtrlTraits<isCompact>::nonemptyStyleClass);

      mGoal = goal;

      render();
   }

   void detach() override
   {
      setStyleClass(GoalViewCtrlTraits<isCompact>::emptyStyleClass);
      GoalViewCtrlTraits<isCompact>::setTitle("", *mBound);
      mGoal.reset();
      if(!isCompact)
      {
         mNotes->setText("");
      }
   }

private:
   void onBoundDoubleClicked()
   {
      if(mGoal)
      {
         GoalEditDialog::TOnOkCallback cb([&] (auto g) {doModifyGoal(g);});
         GoalEditDialog* dlg = new GoalEditDialog(*mGoal, cb);
         dlg->show();
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
      GoalViewCtrlTraits<isCompact>::setTitle(mGoal->title, *mBound);
      if(!isCompact)
      {
         mNotes->setText(mGoal->notes);
      }
   }

   StrategyModel& mModel;
   typename GoalViewCtrlTraits<isCompact>::titleControl* mBound = nullptr;
   Wt::WLabel* mNotes;
   std::optional<StrategyModel::Goal> mGoal;
};

//------------------------------------------------------------------------------------------------------------

class BacklogView : public Wt::WContainerWidget
{
public:
   BacklogView(StrategyModel& model)
   : mModel(model)
   {
      auto activeGroup = addWidget(std::make_unique<Wt::WGroupBox>("Active"));
      auto [temp, activeGoalCtrls] = TemplateBuilder::makeTable<GoalViewCtrl<true>>(1u, 6u, model);
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
      auto [temp2, inactiveGoalCtrls] = TemplateBuilder::makeTable<GoalViewCtrl<true>>(numUnfocused / 6 + 1, 6u, model);
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

//------------------------------------------------------------------------------------------------------------

StrategyView::StrategyView(StrategyModel& strategy)
: mModel(strategy)
{
   setMargin(5);

   auto toolBar = addWidget(std::make_unique<Wt::WToolBar>());
   auto addBtn = std::make_unique<Wt::WPushButton>("Add");
   addBtn->setStyleClass("btn-primary");
   addBtn->clicked().connect(std::bind(&StrategyView::onAddClick, this));
   toolBar->addButton(std::move(addBtn));

   auto backlogBtn = std::make_unique<Wt::WPushButton>("Backlog");
   backlogBtn->setStyleClass("btn-primary");
   backlogBtn->clicked().connect(std::bind(&StrategyView::onBacklogClick, this));
   toolBar->addButton(std::move(backlogBtn));

   auto [temp, goalCtrls] = TemplateBuilder::makeTable<GoalViewCtrl<false>>(3u, 2u, strategy);
   std::copy(goalCtrls.begin(), goalCtrls.end(), std::inserter(mGoalCtrls, mGoalCtrls.begin()));

   addWidget(std::unique_ptr<Wt::WWidget>(temp));

   layGoals();
}

void StrategyView::onAddClick()
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