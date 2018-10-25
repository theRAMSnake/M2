#include "MainScreen.hpp"
#include "InboxView.hpp"
#include "ActionsView.hpp"
#include "StrategyView.hpp"

#include <Wt/WNavigationBar.h>
#include <Wt/WText.h>
#include <Wt/WStackedWidget.h>
#include <Wt/WMenu.h>
#include "MainView.hpp"

MainScreen::MainScreen(MateriaClient& client)
{
    auto navigation = addWidget(Wt::cpp14::make_unique<Wt::WNavigationBar>());
    navigation->setTitle("Materia");

    auto contentsStack = addWidget(Wt::cpp14::make_unique<Wt::WStackedWidget>());
    contentsStack->addStyleClass("contents");
    
    auto menu = Wt::cpp14::make_unique<Wt::WMenu>(contentsStack);
    auto menu_ = navigation->addMenu(std::move(menu));

    menu_->addItem("MainView", Wt::cpp14::make_unique<MainView>(client.getCalendar(), client.getStrategy(), client.getInbox()));
    //menu_->addItem("Strategy", Wt::cpp14::make_unique<StrategyView>());
    //menu_->addItem("Journal", Wt::cpp14::make_unique<JournalView>());
}