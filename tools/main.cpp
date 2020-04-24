#include <iostream>
#include <fstream>
#include "WebApp/materia/ZmqPbChannel.hpp"
#include "WebApp/materia/JournalModel.hpp"
#include "Common/Password.hpp"

int main(int argc,  char** argv)
{
   if(argc < 3)
   {
      std::cout << "Usage m2tools <op> <params>";
      return -1;
   }

   std::string password = materia::getPassword();
   std::string op = argv[1];

   zmq::context_t context(1);
   zmq::socket_t socket(context, ZMQ_REQ);
   ZmqPbChannel channel(socket, "tools", password);

   const std::string ip = "localhost";
   socket.connect("tcp://62.171.175.23:5757");

   JournalModel m(channel);
   if(op == "createPage")
   {
      auto parentId = m.searchIndex(argv[2]);

      if(parentId == materia::Id::Invalid)
      {
         std::cout << "Parent not found";
         return -1;
      }

      std::ifstream f(argv[3], std::ifstream::in);
      if(!f)
      {
         std::cout << "File not found";
         return -1;
      }

      std::string str((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());

      auto indexItem = m.addIndexItem(true, argv[3], parentId);
      m.saveContent(indexItem.id, str);
   }
   else if(op == "loadPage")
   {
      auto id = m.searchIndex(argv[2]);

      if(id == materia::Id::Invalid)
      {
         std::cout << "Page not found";
         return -1;
      }

      std::ofstream f(argv[3], std::ifstream::out | std::ifstream::trunc);
      f << m.loadContent(id);
   }

   std::cout << "Done";
} 