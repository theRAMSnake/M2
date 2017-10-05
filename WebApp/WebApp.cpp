#include "WebApp.hpp"
#include "MainScreen.hpp"

#include <Wt/WBootstrapTheme.h>
#include <Wt/WOverlayLoadingIndicator.h>
#include <Wt/WText.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>

WebApp::WebApp(const Wt::WEnvironment & env)
    : Wt::WApplication(env)
{
    setLoadingIndicator(std::unique_ptr<Wt::WOverlayLoadingIndicator>(new Wt::WOverlayLoadingIndicator()));

    setTitle("Materia");

    std::shared_ptr<Wt::WBootstrapTheme> theme(new Wt::WBootstrapTheme);
    theme->setVersion(Wt::BootstrapVersion::v3);
    setTheme(theme);
    useStyleSheet("resources/bootstrap.css");
    useStyleSheet("custom.css");

    showLogin();
}

void WebApp::showLogin()
{
   mLoginScreen = new Wt::WContainerWidget();

   auto text(new Wt::WText("<p>Password required.</p>"));
   text->addStyleClass("text-center h3");
   mLoginScreen->addWidget(std::unique_ptr<Wt::WText>(text));

   Wt::WLineEdit *edit = new Wt::WLineEdit();
   edit->setEchoMode(Wt::EchoMode::Password);
   edit->setFocus();
   edit->enterPressed().connect(std::bind([=] () {
      onPasswordSent(edit->text());
   }));
   mLoginScreen->addWidget(std::unique_ptr<Wt::WLineEdit>(edit));

   root()->addWidget(std::unique_ptr<Wt::WContainerWidget>(mLoginScreen));
}

void WebApp::onPasswordSent(const Wt::WString& text)
{
   if(text == "test")
   {
      root()->removeWidget(mLoginScreen);
      showMainScreen();
   }
}

void WebApp::showMainScreen()
{
   root()->addWidget(std::unique_ptr<Wt::WContainerWidget>(new MainScreen()));
}