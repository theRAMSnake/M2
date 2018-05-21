#include <zmq.hpp>
#include <iostream>

void start()
{
   system("nohup ./m2Container &");
   system("nohup ./m2InboxService &");
   system("nohup ./m2ActionsService &");
   system("nohup ./m2Calendar &");
   system("nohup ./m2Events &");
   system("nohup ./m2Strategy &");
   system("nohup ./m2central &");
   system("nohup ./run_wa.sh &");

   std::cout << "Done\n";
}

void stop()
{
   system("pkill m2Container");
   system("pkill m2DatabaseServi");
   system("pkill m2InboxService");
   system("pkill m2ActionsServic");
   system("pkill m2Calendar");
   system("pkill m2Events");
   system("pkill m2Strategy");
   system("pkill m2central");
   system("pkill WebApp");

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
   else
   {
      std::cout << "Unknown command " << commandName.c_str();
      return -1;
   }
   
   return 0;
}
