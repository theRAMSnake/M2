#include <iostream>
#include <fstream>
#include "WebApp/materia/ZmqPbChannel.hpp"
#include "WebApp/materia/MateriaServiceProxy.hpp"
#include <messages/admin.pb.h>
#include <messages/inbox.pb.h>
#include <Common/Password.hpp>

std::string genName()
{
   time_t t;
   return "materia.bu." + std::to_string(time(&t));
}

std::string genName3()
{
   time_t t;
   return "materia.bu3." + std::to_string(time(&t));
}

int main(int argc,  char** argv)
{
   std::string password = materia::getPassword();

   zmq::context_t context(1);
   zmq::socket_t socket(context, ZMQ_REQ);
   ZmqPbChannel channel(socket, "backuper", password);

   const std::string ip = "localhost";
   socket.connect("tcp://62.171.175.23:5757");

   MateriaServiceProxy<admin::AdminService> service(channel);

   {
      common::EmptyMessage empty;
      admin::BackupChunk chunk;
      service.getService().StartBackup(nullptr, &empty, &chunk, nullptr);

      std::ofstream stream;
      auto fname = genName();
      stream.open(fname, std::ios::out | std::ios::binary | std::ios::trunc);

      stream.write(&chunk.bytes().front(), chunk.bytes().size());
      std::cout << "Chunk: " << chunk.bytes().size() << "\n";

      while(chunk.has_more())
      {
         service.getService().Next(nullptr, &empty, &chunk, nullptr);
         stream.write(&chunk.bytes().front(), chunk.bytes().size());
         std::cout << "Chunk: " << chunk.bytes().size() << "\n";
      }

      stream.close();
   }

   MateriaServiceProxy<inbox::InboxService> ib(channel);

   auto now = time(0);
   tm* ltm = localtime(&now);

   inbox::InboxItemInfo itemToAdd;
   auto text = "Backup was made on " + std::to_string(ltm->tm_mday) + "/" + std::to_string(1+ ltm->tm_mon) +
      " at " + std::to_string(ltm->tm_hour);
   itemToAdd.set_text(text);

   common::UniqueId id;
   ib.getService().AddItem(nullptr, &itemToAdd, &id, nullptr);
} 