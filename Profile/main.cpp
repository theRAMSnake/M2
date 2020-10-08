#include <iostream>
#include <chrono>
#include <fstream>
#include <Core/ICore3.hpp>
#include <vector>
#include <string>
#include <map>

int main(int argc,  char** argv)
{
   std::map<std::string, std::vector<int>> result;
   std::ifstream stream("/home/snake/Work/cache_dump.txt");
   std::string feat;
   int order;
   int frame;
   while(stream >> frame >> feat >> order)
   {
      result[feat].push_back(order);
   }

   for(auto x : result)
   {
      std::cout << x.first << ": ";
      for(auto y : x.second)
      {
         std::cout << y << " ";
      }
      std::cout << std::endl;
   }
}