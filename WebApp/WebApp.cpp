#include "WebApp.hpp"
#include "MainScreen.hpp"
#include "SharedFilesView.hpp"

#include <Wt/WBootstrapTheme.h>
#include <Wt/WOverlayLoadingIndicator.h>
#include <Wt/WText.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WLabel.h>

#include <future>
#include <zmq.hpp>
#include <messages/common.pb.h>

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

   if(internalPath() == "/files")
   {
      root()->addWidget(std::make_unique<SharedFilesView>());
      return;
   }
   else
   {
      showLogin();
   }
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
   root()->removeWidget(mLoginScreen);
   if(checkMateriaAvailabilityAndPassword(text.narrow()))
   {
      mClient.reset(new MateriaClient(text.narrow()));
      showMainScreen();
   }
   else
   {
      showErrorScreen();
   }
}

bool WebApp::checkMateriaAvailabilityAndPassword(const std::string& str)
{
   zmq::context_t context;
   zmq::socket_t socket(context, ZMQ_REQ);
   socket.setsockopt(ZMQ_LINGER, 0);

   socket.connect("tcp://localhost:5757");

   common::MateriaMessage envelope;
   envelope.set_from("wa");
   envelope.set_to("AdminService");
   envelope.set_operationname("wrongopname");
   
   zmq::message_t req (envelope.ByteSizeLong());
   envelope.SerializeToArray(req.data (), req.size());
   
   socket.send (req);
   
   zmq::message_t resp;

   auto start = std::chrono::system_clock::now();
   while(!socket.recv (&resp, ZMQ_NOBLOCK))
   {
      auto now = std::chrono::system_clock::now();
      
      auto duration = now - start;
      if(duration > std::chrono::milliseconds(3000))
      {
         return false;
      }
   }

   //materia available, lets check password
   MateriaClient client(str);
   return !client.getJournal().getChildren(materia::Id::Invalid).empty();
}

void WebApp::showMainScreen()
{
    root()->addWidget(std::unique_ptr<Wt::WContainerWidget>(new MainScreen(*mClient)));
}

void WebApp::showErrorScreen()
{
   root()->addWidget(std::unique_ptr<Wt::WText>(new Wt::WText("Materia connection is unavailable")));
}