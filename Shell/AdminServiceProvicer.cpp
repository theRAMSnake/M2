#include "AdminServiceProvider.hpp"
#include <boost/asio.hpp>
namespace ip = boost::asio::ip;

namespace materia
{
   
AdminServiceProvider::AdminServiceProvider(ZmqPbChannel& channel)
: mService(&channel)
{
    boost::asio::io_service io_service;
    ip::tcp::resolver resolver(io_service);
 
    mSenderName = ip::host_name();
}
   
std::string AdminServiceProvider::execute(const std::string& input)
{
   std::string opName;
   std::size_t delimPos = input.find(' ');
   if(delimPos == -1)
   {
      opName = input;
   }
   else
   {
      opName = input.substr(0, delimPos);
   }
   
   if(opName == "getComponentInfo")
   {
      return getComponentInfo();
   }
   else
   {
      return "Unsupported operation";
   }
}

std::string AdminServiceProvider::getComponentInfo()
{
   common::MateriaMessage msg;
   msg.set_from(mSenderName);
   msg.set_to("admin");
   msg.set_operationname("getComponentInfo");
   
   admin::ComponentInfoResponce responce;
   mService.GetComponentInfo(0, &msg, &responce, 0);
   
   return responce.DebugString();
}
   
}
