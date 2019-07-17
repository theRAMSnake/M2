#include <zmq.hpp>
#include <iostream>
#include "WebApp/materia/ZmqPbChannel.hpp"
#include "WebApp/materia/MateriaServiceProxy.hpp"
#include <messages/admin.pb.h>

void start()
{
   system("nohup ./m2server &");
   system("nohup ./run_wa.sh &");

   std::cout << "Done\n";
}

void stop()
{
   system("pkill WebApp");

   zmq::context_t context(1);
   zmq::socket_t socket(context, ZMQ_REQ);
   ZmqPbChannel channel(socket, "backuper");

   const std::string ip = "localhost";
   socket.connect("tcp://localhost:5757");

   MateriaServiceProxy<admin::AdminService> service(channel);

   common::EmptyMessage empty;
   service.getService().ShutDownCore(nullptr, &empty, &empty, nullptr);

   std::cout << "Done\n";
}

void test()
{
   system("nohup ./m2server &");
   system("nohup ./run_wa_test.sh & > wa.log");

   std::cout << "Done\n";
}

int main(int argc, char *argv[])
{
   if(argc < 2)
   {
      std::cout << "Please specify command to run";
      return -1;
   }

   std::string commandName = argv[1];
   if(commandName == "start")
   {
      start();
   }
   else if(commandName == "stop")
   {
      stop();
   }
   else if(commandName == "test")
   {
      test();
   }
   else
   {
      std::cout << "Unknown command " << commandName.c_str();
      return -1;
   }
   
   return 0;
}
