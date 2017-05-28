#pragma once
#include <Wt/WApplication>

class WebApp : public Wt::WApplication
{
public:
    WebApp(const Wt::WEnvironment& env);

private:
   
    void showLogin();
    void showMainScreen();
    void onPasswordSent(const Wt::WString& text);

    Wt::WContainerWidget* mMainScreen;
    Wt::WContainerWidget* mLoginScreen;
};