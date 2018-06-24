#pragma once
#include <Wt/WApplication.h>
#include <Client/MateriaClient.hpp>

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
    materia::MateriaClient mClient;
};