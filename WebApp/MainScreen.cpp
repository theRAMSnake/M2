#include "MainScreen.hpp"
#include "InboxView.hpp"
#include "ActionsView.hpp"

#include <Wt/WNavigationBar.h>
#include <Wt/WText.h>
#include <Wt/WStackedWidget.h>
#include <Wt/WMenu.h>

MainScreen::MainScreen(materia::MateriaClient& client)
{
    auto navigation = addWidget(Wt::cpp14::make_unique<Wt::WNavigationBar>());
    navigation->setTitle("Materia");

    auto contentsStack = addWidget(Wt::cpp14::make_unique<Wt::WStackedWidget>());
    contentsStack->addStyleClass("contents");
    
    auto menu = Wt::cpp14::make_unique<Wt::WMenu>(contentsStack);
    auto menu_ = navigation->addMenu(std::move(menu));

    menu_->addItem("Actions", Wt::cpp14::make_unique<ActionsView>(client.getActions()));
    menu_->addItem("Strategy", Wt::cpp14::make_unique<StrategyView>(client.getStrategy(), client.getCalendar()));
}