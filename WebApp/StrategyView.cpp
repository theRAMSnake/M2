#include "StrategyView.hpp"
#include "CommonDialogManager.hpp"
#include "TemplateBuilder.hpp"
#include "Wt/WToolBar.h"
#include "Wt/WPushButton.h"
#include "Wt/WLabel.h"

class GoalViewCtrl : public Wt::WContainerWidget
{
public:
   GoalViewCtrl()
   {
      setStyleClass("GoalViewCtrl");

      addWidget(std::make_unique<Wt::WLabel>("a"));
   }
};

//------------------------------------------------------------------------------------------------------------

StrategyView::StrategyView(StrategyModel& strategy)
: mModel(strategy)
{
   setMargin(5);

   auto toolBar = addWidget(std::make_unique<Wt::WToolBar>());
   auto btn = std::make_unique<Wt::WPushButton>("Add");
   btn->setStyleClass("btn-primary");
   btn->clicked().connect(std::bind(&StrategyView::onAddClick, this));

   toolBar->addButton(std::move(btn));

   auto [temp, mGoalCtrls] = TemplateBuilder::makeTable<GoalViewCtrl>(3u, 2u);

   addWidget(std::unique_ptr<Wt::WWidget>(temp));

   for(auto g : mModel.getGoals())
   {
      if(g.focused)
      {
         putGoal(g);
      }
   }
}

void StrategyView::onAddClick()
{
   std::vector<std::string> choise = {"Active", "Backlog"};
      CommonDialogManager::showChoiseDialog(choise, [=](auto selected) {
         const bool isActive = selected == 0;

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

}