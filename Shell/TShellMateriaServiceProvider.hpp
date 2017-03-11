#pragma once
#include <string>
#include <boost/algorithm/string.hpp>
#include "MessageFactory.hpp"

namespace materia
{
   
class IMateriaServiceProvider
{
public:
   virtual std::string execute(const std::string& input) = 0;
};
   
template<class TService>
class TShellMateriaServiceProvider : public IMateriaServiceProvider
{
public:
   TShellMateriaServiceProvider(ZmqPbChannel& channel)
   : mService(&channel)
   {
   }
   
   virtual std::string execute(const std::string& input)
   {
      auto method = mService.descriptor()->FindMethodByName(input);
      if(!method)
      {
         return "Unsupported operation";
      }
      
      std::unique_ptr<google::protobuf::Message> req(MessageFactory::queryMessageFromUser(method->input_type()->name()));
      if(req.get() == nullptr)
      {
         return "Failed to create " + method->input_type()->full_name() + " from input";
      }
      
      std::unique_ptr<google::protobuf::Message> resp(mService.GetResponsePrototype(method).New());
      mService.CallMethod(method, nullptr, req.get(), resp.get(), nullptr);
      return resp->DebugString();
   }
   
private:
   typename TService::Stub mService;
};
   
}
