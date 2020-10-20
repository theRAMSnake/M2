#include <iostream>
#include <fstream>
#include "Common/Password.hpp"
#include "Common/Codec.hpp"
#include <zmq.hpp>

class Channel
{
public:
   Channel(zmq::socket_t& zmqSocket, const std::string& password)
   : mZmqSocket(zmqSocket)
   , mCodec(password)
   {

   }

   std::string send(const std::string& cmd)
   {
      std::string encrypted = mCodec.encrypt(cmd);
      zmq::message_t req (encrypted.data(), encrypted.size());
      mZmqSocket.send (req, zmq::send_flags::none);

      zmq::message_t resp;
      mZmqSocket.recv (resp, zmq::recv_flags::none);

      std::string received(static_cast<const char *>(resp.data()), resp.size());
      
      return mCodec.decrypt(received);      
   }

private:
   zmq::socket_t& mZmqSocket;
   Codec mCodec;
};

int main(int argc,  char** argv)
{
   if(argc < 2)
   {
      std::cout << "Usage m3tools <op>";
      return -1;
   }

   std::string password = materia::getPassword();
   std::string op = argv[1];

   zmq::context_t context(1);
   zmq::socket_t socket(context, ZMQ_REQ);
   socket.connect("tcp://ramsnake.net:5756");
   Channel channel(socket, password);

   std::cout << channel.send(op);

   return 0;
} 