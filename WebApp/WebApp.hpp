#pragma once
#include <Wt/WApplication.h>

class WebApp : public Wt::WApplication
{
public:
    WebApp(const Wt::WEnvironment& env);

private:
   
    void showLogin();
    void showMainScreen();
    void showErrorScreen();
    void onPasswordSent(const Wt::WString& text);
    bool checkMateriaAvailability();

    Wt::WContainerWidget* mMainScreen;
    Wt::WContainerWidget* mLoginScreen;
};