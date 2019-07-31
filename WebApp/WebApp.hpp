#pragma once
#include <Wt/WApplication.h>
#include "MateriaClient.hpp"

class WebApp : public Wt::WApplication
{
public:
    WebApp(const Wt::WEnvironment& env);

private:
   
    void showLogin();
    void showMainScreen();
    void showErrorScreen();
    void onPasswordSent(const Wt::WString& text);
    bool checkMateriaAvailabilityAndPassword(const std::string& str);

    Wt::WContainerWidget* mMainScreen;
    Wt::WContainerWidget* mLoginScreen;
    std::unique_ptr<MateriaClient> mClient;
};