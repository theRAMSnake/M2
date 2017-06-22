#include "MainScreen.hpp"
#include "InboxView.hpp"

#include <Wt/WNavigationBar>
#include <Wt/WText>
#include <Wt/WStackedWidget>
#include <Wt/WMenu>

MainScreen::MainScreen()
{
    Wt::WNavigationBar *navigation = new Wt::WNavigationBar(this);
    navigation->setTitle("Materia");

    Wt::WStackedWidget *contentsStack = new Wt::WStackedWidget(this);
    contentsStack->addStyleClass("contents");
    
    Wt::WMenu *leftMenu = new Wt::WMenu(contentsStack, this);

    leftMenu->addItem("Inbox", new InboxView());
    leftMenu->addItem("Actions", new Wt::WText("actions"));
    
    navigation->addMenu(leftMenu);
}