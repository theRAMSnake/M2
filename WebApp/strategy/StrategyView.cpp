#include "StrategyView.hpp"
#include "GoalViewCtrl.hpp"
#include "../dialog/CommonDialogManager.hpp"
#include "../dialog/Dialog.hpp"
#include "../TemplateBuilder.hpp"
#include "../TaskEditDialog.hpp"
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

//------------------------------------------------------------------------------------------------------------

class BacklogView : public Wt::WContainerWidget
{
public:
   BacklogView(StrategyModel& model)
   : mModel(model)
   {
      auto activeGroup = addWidget(std::make_unique<Wt::WGroupBox>("Active"));
      auto [temp, activeGoalCtrls] = TemplateBuilder::makeTable<GoalViewCtrl<true>>(1u, 4u, GoalViewCtrlConstructionParams{model, true});
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

//-----------------------------------------------------------------------------------------------------------

class WatchItemCtrl : public Wt::WContainerWidget
{
public:
   WatchItemCtrl(const StrategyModel::WatchItem& w, StrategyModel& model)
   : mModel(model)
   , mItem(w)
   {
      mText = addWidget(std::make_unique<Wt::WLabel>(w.title));
      mText->setInline(true);
      setStyleClass("WatchItemCtrl");

      clicked().connect(this, &WatchItemCtrl::onClicked);
   }

private:
   void onClicked(Wt::WMouseEvent event)
   {
      if(event.button() == Wt::MouseButton::Left)
      {
         if(event.modifiers().test(Wt::KeyboardModifier::Control))
         {
            std::function<void()> elementDeletedFunc = [=] () {
               mModel.deleteWatchItem(mItem.id);
               parent()->removeChild(this);
            };

            CommonDialogManager::showConfirmationDialog("Delete it?", elementDeletedFunc);
         }
         else
         {
            std::function<void(std::string)> nextFunc = [=](std::string name){
                onEditDialogOk(name);
            };

            CommonDialogManager::showOneLineDialog("Text", "Text", mItem.title, nextFunc);
         }
      }
   }

   void onEditDialogOk(const std::string& newText)
   {
      mText->setText(newText);
      mItem.title = newText;
      mModel.modifyWatchItem(mItem);
   } 

   Wt::WLabel* mText;
   StrategyModel& mModel;
   StrategyModel::WatchItem mItem;
};

//------------------------------------------------------------------------------------------------------------

StrategyView::StrategyView(StrategyModel& strategy)
: mModel(strategy)
{
   setMargin(5);

   mMainToolbar = addWidget(std::make_unique<Wt::WToolBar>());

   auto popupPtr = std::make_unique<Wt::WPopupMenu>();
   popupPtr->addItem("Goal")->triggered().connect(std::bind(&StrategyView::onAddGoalClick, this));
   popupPtr->addItem("Resource")->triggered().connect(std::bind(&StrategyView::onAddResourceClick, this));
   popupPtr->addItem("Objective")->triggered().connect(std::bind(&StrategyView::onAddObjectiveClick, this));
   popupPtr->addItem("Watch Item")->triggered().connect(std::bind(&StrategyView::onAddWatchItemClick, this));

   auto addBtn = std::make_unique<Wt::WPushButton>("Add");
   addBtn->setStyleClass("btn-primary");
   addBtn->setMenu(std::move(popupPtr));
   mMainToolbar->addButton(std::move(addBtn));

   auto backlogBtn = std::make_unique<Wt::WPushButton>("Backlog");
   backlogBtn->setStyleClass("btn-primary");
   backlogBtn->clicked().connect(std::bind(&StrategyView::onBacklogClick, this));
   mMainToolbar->addButton(std::move(backlogBtn));

   auto [temp, goalCtrls] = TemplateBuilder::makeTable<GoalViewCtrl<false>>(2u, 2u, GoalViewCtrlConstructionParams{strategy, true});
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

   for(auto w : mModel.getWatchItems())
   {
      toolbar.addWidget(std::make_unique<WatchItemCtrl>(w, mModel));
   }

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

void StrategyView::onAddWatchItemClick()
{     
   std::function<void(std::string)> nextFunc = [=](std::string name){
      auto item = mModel.addWatchItem(name);
      
      mMainToolbar->addWidget(std::make_unique<WatchItemCtrl>(item, mModel));
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
   Dialog* dlg = new Dialog("Backlog View", std::make_unique<BacklogView>(mModel));
   dlg->OnFinished.connect(std::bind([=]() {
         layGoals();
   }));
   dlg->show();
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