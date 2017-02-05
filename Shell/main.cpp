#include <zmq.hpp>
#include <iostream>
#include <string>
#include <messages/common.pb.h>
#include <Common/ZmqPbChannel.hpp>

#include "AdminServiceProvider.hpp"

int main(int argc, char *argv[])
{
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
   
   materia::ZmqPbChannel channel(socket);
   
   std::map<std::string, std::shared_ptr<materia::IMateriaServiceProvider>> providers;
   providers.insert(std::make_pair("admin", new materia::AdminServiceProvider(channel)));

   while(true)
   {
      std::string userText;
      std::cin >> userText;
      
      std::size_t pointPos = userText.find('.');
      if(pointPos == -1)
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
