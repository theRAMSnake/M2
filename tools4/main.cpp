#include <iostream>
#include <fstream>
#include <stdio.h>
#include <array>
#include <memory>
#include "Common/Password.hpp"
#include "Common/Codec.hpp"
#include "Common/Base64.hpp"

std::string bash(const std::string& in)
{
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(in.c_str(), "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

class Channel
{
public:
   Channel(const std::string& password)
   : mCodec(password)
   {

   }

   std::string send(const std::string& cmd)
   {
      std::string encrypted = base64_encode(mCodec.encrypt(cmd));

      std::string addr = "http://ramsnake.net:5754/api";
      std::string curlcmd = "curl -s -X POST --data '" + encrypted + "' " + addr;
      
      return base64_decode(mCodec.decrypt(bash(curlcmd)));      
   }

private:
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

   Channel channel(password);

   std::cout << channel.send(op);

   return 0;
} 
