#pragma once
#include <string>
#include <boost/algorithm/string.hpp>
#include "MessageFactory.hpp"

namespace materia
{
   
class IMateriaServiceProvider
{
public:
   virtual std::string execute(const int methodIndex) = 0;
   virtual std::vector<std::string> getMethodNames() const = 0;
};
   
template<class TService>
class TShellMateriaServiceProvider : public IMateriaServiceProvider
{
public:
   TShellMateriaServiceProvider(ZmqPbChannel& channel)
   : mService(&channel)
   {
   }
   
   virtual std::string execute(const int methodIndex)
   {
      auto method = mService.descriptor()->method(methodIndex);
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

   virtual std::vector<std::string> getMethodNames() const
   {
      std::vector<std::string> result;

      auto desc = mService.descriptor();
      for(int i = 0; i < desc->method_count(); ++i)
      {
         result.push_back(desc->method(i)->name());
      }

      return result;
   }
   
private:
   typename TService::Stub mService;
};
   
}
