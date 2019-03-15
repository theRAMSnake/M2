#include "MainScreen.hpp"
#include "InboxView.hpp"
#include "ActionsView.hpp"
#include "StrategyView.hpp"
#include "JournalView.hpp"
#include "RewardsSmartPage.hpp"

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

    RewardsSmartPage::update(client.getJournal(), client.getStrategy());

    menu_->addItem("MainView", std::make_unique<MainView>(
        client.getCalendar(), 
        client.getStrategy(),
        client.getInbox(),
        client.getJournal()));
    menu_->addItem("Journal", std::make_unique<JournalView>(client.getJournal()));
    menu_->addItem("Strategy", std::make_unique<StrategyView>(client.getStrategy()));
}