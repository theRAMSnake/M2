#include <iostream>
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

   auto cmd = "{\"operation\":\"query\",\"filter\":\"IS(calendar_item)\"}";

   for(int i = 0; i < 1000; ++i)
   {
       if(i % 100 == 0)
       {
           std::cout << ".";
           std::cout.flush();
       }
       core->executeCommandJson(cmd);
   }

   std::cout << "Done";
} 