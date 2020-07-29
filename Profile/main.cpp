#include <iostream>
#include <chrono>
#include <Core/ICore3.hpp>

int main(int argc,  char** argv)
{
   if(argc < 2)
   {
      std::cout << "Usage Profile <dbpath>";
      return -1;
   }

   std::string path = argv[1];

   auto core = materia::createCore({path});
   core->onNewDay();

   /*auto cmd = "{\"operation\":\"query\",\"filter\":\"IS(calendar_item)\"}";

   std::chrono::time_point<std::chrono::high_resolution_clock> started = std::chrono::high_resolution_clock::now();

   for(int i = 0; i < 1000; ++i)
   {
       if(i % 100 == 0)
       {
           std::cout << ".";
           std::cout.flush();
       }
       core->executeCommandJson(cmd);
   }

   auto time_D_msec = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - started);

   std::cout << "Done " + std::to_string(time_D_msec.count()) + "ms";*/

   //741
} 