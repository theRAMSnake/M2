#include "MainScreen.hpp"
#include "InboxView.hpp"
#include "FilesView.hpp"
#include "FinanceView.hpp"
#include "strategy/StrategyView.hpp"
#include "JournalView.hpp"
#include "RewardView.hpp"

#include <Wt/WNavigationBar.h>
#include <Wt/WText.h>
#include <Wt/WStackedWidget.h>
#include <Wt/WMenu.h>
#include "MainView.hpp"

MainScreen::MainScreen(MateriaClient& client)
{
    auto navigation = addWidget(std::make_unique<Wt::WNavigationBar>());
    navigation->setTitle("Materia");

    auto contentsStack = addWidget(std::make_unique<Wt::WStackedWidget>());
    contentsStack->addStyleClass("contents");
    
    auto menu = std::make_unique<Wt::WMenu>(contentsStack);
    auto menu_ = navigation->addMenu(std::move(menu));

    menu_->addItem("MainView", std::make_unique<MainView>(
        client.getCalendar(), 
        client.getStrategy(),
        client.getInbox(),
        client.getJournal()));
    menu_->addItem("Journal", std::make_unique<JournalView>(client.getJournal()));
    menu_->addItem("Strategy", std::make_unique<StrategyView>(client.getStrategy(), client.getFreeData()));
    menu_->addItem("Files", std::make_unique<FilesView>());
    menu_->addItem("Finance", std::make_unique<FinanceView>(client.getFinance()));
    menu_->addItem("Reward", std::make_unique<RewardView>(client.getReward()));
}