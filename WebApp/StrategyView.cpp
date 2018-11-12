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

class GoalViewCtrl : public Wt::WContainerWidget
{
public:
   GoalViewCtrl()
   {
      setStyleClass("GoalViewCtrlEmpty");

      mText = addWidget(std::make_unique<Wt::WLabel>(""));
      mText->setStyleClass("GoalViewCtrl_Text");
   }

   bool isEmpty() const
   {
      return !mGoal;
   }

   void attach(const StrategyModel::Goal& goal)
   {
      setStyleClass("GoalViewCtrl");

      mGoal = goal;

      render();
   }

   void detach()
   {
      setStyleClass("GoalViewCtrlEmpty");
      mText->setText("");
      mGoal.reset();
   }

private:
   void render()
   {
      mText->setText(mGoal->title);
   }

   Wt::WLabel* mText = nullptr;
   std::optional<StrategyModel::Goal> mGoal;
};

//------------------------------------------------------------------------------------------------------------

class GoalViewCtrlSmall : public Wt::WContainerWidget
{
public:
   GoalViewCtrlSmall()
   {
      setStyleClass("GoalViewCtrlSmallEmpty");

      mText = addWidget(std::make_unique<Wt::WLabel>(""));
      mText->setStyleClass("GoalViewCtrl_Text");
   }

   bool isEmpty() const
   {
      return !mGoal;
   }

   void attach(const StrategyModel::Goal& goal)
   {
      setStyleClass("GoalViewCtrlSmall");

      mGoal = goal;

      render();
   }

   void detach()
   {
      setStyleClass("GoalViewCtrlSmallEmpty");
      mText->setText("");
      mGoal.reset();
   }

private:
   void render()
   {
      mText->setText(mGoal->title);
   }

   Wt::WLabel* mText = nullptr;
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
      auto [temp, activeGoalCtrls] = TemplateBuilder::makeTable<GoalViewCtrlSmall>(1u, 6u);
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
      auto [temp2, inactiveGoalCtrls] = TemplateBuilder::makeTable<GoalViewCtrlSmall>(numUnfocused / 6 + 1, 6u);
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

   auto [temp, goalCtrls] = TemplateBuilder::makeTable<GoalViewCtrl>(3u, 2u);
   mGoalCtrls = goalCtrls;

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