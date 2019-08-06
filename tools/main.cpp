#include <iostream>
#include <fstream>
#include "WebApp/materia/ZmqPbChannel.hpp"
#include "WebApp/materia/JournalModel.hpp"

int main(int argc,  char** argv)
{
   if(argc != 4)
   {
      std::cout << "Usage m2tools <password> <parent> <doc_name>";
      return -1;
   }

   std::string password = argv[1];

   zmq::context_t context(1);
   zmq::socket_t socket(context, ZMQ_REQ);
   ZmqPbChannel channel(socket, "tools", password);

   const std::string ip = "localhost";
   socket.connect("tcp://188.116.57.62:5757");

   JournalModel m(channel);

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

   std::cout << "Done";
} 