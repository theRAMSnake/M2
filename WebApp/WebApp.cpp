#include "WebApp.hpp"
#include "MainScreen.hpp"

#include <Wt/WBootstrapTheme>
#include <Wt/WOverlayLoadingIndicator>
#include <Wt/WText>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>

WebApp::WebApp(const Wt::WEnvironment & env)
    : Wt::WApplication(env)
{
    setLoadingIndicator(new Wt::WOverlayLoadingIndicator());

    setTitle("Materia");

    Wt::WBootstrapTheme* theme = new Wt::WBootstrapTheme;
    theme->setVersion(Wt::WBootstrapTheme::Version3);
    setTheme(theme);
    useStyleSheet("resources/bootstrap.css");

    showLogin();
}

void WebApp::showLogin()
{
   mLoginScreen = new Wt::WContainerWidget();

   auto text = new Wt::WText("<p>Password required.</p>");
   text->addStyleClass("text-center h3");
   mLoginScreen->addWidget(text);

   Wt::WLineEdit *edit = new Wt::WLineEdit();
   edit->setEchoMode(Wt::WLineEdit::EchoMode::Password);
   edit->setFocus();
   edit->enterPressed().connect(std::bind([=] () {
      onPasswordSent(edit->text());
   }));
   mLoginScreen->addWidget(edit);

   root()->addWidget(mLoginScreen);
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
   root()->addWidget(new MainScreen());
}