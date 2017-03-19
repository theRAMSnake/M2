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
   
   socket.connect((std::string("tcp://") + address + ":5910").c_str());
   
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
      std::cout << "Service: " << std::endl;

      int num = 0;
      for(auto x : providers)
      {
         std::cout << "\t " << ++num << " - " << x.first << std::endl;
      }

      std::size_t userChoise;
      std::cin >> userChoise;

      if(userChoise < 1 || userChoise > providers.size())
      {
         std::cout << "Invalid choise" << std::endl;
         continue;
      }     
      
      auto iter = providers.begin();
      std::advance(iter, userChoise - 1);
      std::vector<std::string> methodNames = iter->second->getMethodNames();

      std::cout << "Method: " << std::endl;

      num = 0;
      for(auto x : methodNames)
      {
         std::cout << "\t " << ++num << " - " << x << std::endl;
      }

      std::cin >> userChoise;

      if(userChoise < 1 || userChoise > methodNames.size())
      {
         std::cout << "Invalid choise" << std::endl;
         continue;
      }
      
      std::cout << iter->second->execute(userChoise - 1) << std::endl;
   }
   
   return 0;
}
