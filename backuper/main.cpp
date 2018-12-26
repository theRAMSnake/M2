#include <iostream>
#include <fstream>
#include "WebApp/materia/ZmqPbChannel.hpp"
#include "WebApp/materia/MateriaServiceProxy.hpp"
#include <messages/admin.pb.h>

int main(int argc,  char** argv)
{
   zmq::context_t context(1);
   zmq::socket_t socket(context, ZMQ_REQ);
   ZmqPbChannel channel(socket, "backuper");

   const std::string ip = "localhost";
   socket.connect("tcp://localhost:5757");

   MateriaServiceProxy<admin::AdminService> service(channel);

   common::EmptyMessage empty;
   admin::BackupChunk chunk;
   service.getService().StartBackup(nullptr, &empty, &chunk, nullptr);

   std::ofstream stream;
   stream.open("materia.backup", std::ios::out | std::ios::binary | std::ios::trunc);

   stream.write(&chunk.bytes().front(), chunk.bytes().size());
   std::cout << "Chunk: " << chunk.bytes().size() << "\n";

   while(chunk.has_more())
   {
      service.getService().Next(nullptr, &empty, &chunk, nullptr);
      stream.write(&chunk.bytes().front(), chunk.bytes().size());
      std::cout << "Chunk: " << chunk.bytes().size() << "\n";
   }

   stream.close();
   std::cout << "Done";
} 