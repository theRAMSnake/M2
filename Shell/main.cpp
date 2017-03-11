#include <zmq.hpp>
#include <iostream>
#include <string>
#include <messages/common.pb.h>
#include <messages/admin.pb.h>
#include <messages/actions.pb.h>
#include <messages/inbox.pb.h>
#include <Common/ZmqPbChannel.hpp>
#include "TShellMateriaServiceProvider.hpp"
#include "MessageFactory.hpp"
#include <boost/asio.hpp>

namespace ip = boost::asio::ip;

void errorCallback(const std::string& str)
{
   std::cout << "Error: " << str.c_str() << std::endl;
}

int main(int argc, char *argv[])
{
   using namespace std::placeholders;
   materia::MessageFactory::init();
   std::string address = "localhost";
   if(argc == 2)
   {
      address = argv[1];
   }
   
   const int NUM_THREADS = 1;
   zmq::context_t context(NUM_THREADS);
   zmq::socket_t socket (context, ZMQ_REQ);
   
   socket.connect((std::string("tcp://") + address + ":5000").c_str());
   
   if(!socket.connected())
   {
      std::cout << "Cannot connect!";
   }
   
   boost::asio::io_service io_service;
   ip::tcp::resolver resolver(io_service);
 
   materia::ZmqPbChannel channel(socket, ip::host_name());
   channel.setErrorCallback(std::bind(&errorCallback, _1));
   
   std::map<std::string, std::shared_ptr<materia::IMateriaServiceProvider>> providers;
   providers.insert(std::make_pair("admin", new materia::TShellMateriaServiceProvider<admin::AdminService>(channel)));
   providers.insert(std::make_pair("inbox", new materia::TShellMateriaServiceProvider<inbox::InboxService>(channel)));
   providers.insert(std::make_pair("actions", new materia::TShellMateriaServiceProvider<actions::ActionsService>(channel)));

   while(true)
   {
      std::string userText;
      std::cin >> userText;
      
      std::size_t pointPos = userText.find('.');
      if(pointPos == std::string::npos)
      {
         std::cout << "Invalid input: expected \'componentName.operationName\'" << std::endl;
         continue;
      }
      
      std::string cmpName = userText.substr(0, pointPos);
      auto iter = providers.find(cmpName);
      if(iter == providers.end())
      {
         std::cout << "Invalid input: component not found" << std::endl;
         continue;
      }
      
      std::cout << iter->second->execute(userText.substr(pointPos + 1, -1)) << std::endl;
   }
   
   return 0;
}
