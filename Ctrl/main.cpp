#include <zmq.hpp>
#include <iostream>
#include <Common/Password.hpp>

void start(const std::string password)
{
   system(("nohup ./m3server " + password + "&").c_str());
   system("WebApp2/run.sh");

   std::cout << "Done\n";
}

void stop(const std::string password)
{
   system("killall node");
   system("./m3tools shutdown");

   std::cout << "Done\n";
}

int main(int argc, char *argv[])
{
   if(argc < 2)
   {
      std::cout << "Usage m3ctrl <command>";
      return -1;
   }

   std::string password = materia::getPassword();
   std::string commandName = argv[1];
   if(commandName == "start")
   {
      start(password);
   }
   else if(commandName == "stop")
   {
      stop(password);
   }
   else if(commandName == "restart")
   {
      stop(password);
      start(password);
   }
   else
   {
      std::cout << "Unknown command " << commandName.c_str();
      return -1;
   }
   
   return 0;
}
