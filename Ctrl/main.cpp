#include <zmq.hpp>
#include <iostream>
#include "WebApp/materia/ZmqPbChannel.hpp"
#include "WebApp/materia/MateriaServiceProxy.hpp"
#include <messages/admin.pb.h>

void start(const std::string password)
{
   system(("nohup ./m2server " + password + "&").c_str());
   system("nohup ./run_wa.sh &");

   std::cout << "Done\n";
}

void stop(const std::string password)
{
   system("pkill WebApp");

   zmq::context_t context(1);
   zmq::socket_t socket(context, ZMQ_REQ);
   ZmqPbChannel channel(socket, "backuper", password);

   const std::string ip = "localhost";
   socket.connect("tcp://localhost:5757");

   MateriaServiceProxy<admin::AdminService> service(channel);

   common::EmptyMessage empty;
   service.getService().ShutDownCore(nullptr, &empty, &empty, nullptr);

   std::cout << "Done\n";
}

void test(const std::string password)
{
   system(("nohup ./m2server " + password + "&").c_str());
   system("nohup ./run_wa_test.sh & > wa.log");

   std::cout << "Done\n";
}

int main(int argc, char *argv[])
{
   if(argc < 3)
   {
      std::cout << "Usage m2ctrl <password> <command>";
      return -1;
   }

   std::string password = argv[1];
   std::string commandName = argv[2];
   if(commandName == "start")
   {
      start(password);
   }
   else if(commandName == "stop")
   {
      stop(password);
   }
   else if(commandName == "test")
   {
      test(password);
   }
   else
   {
      std::cout << "Unknown command " << commandName.c_str();
      return -1;
   }
   
   return 0;
}
