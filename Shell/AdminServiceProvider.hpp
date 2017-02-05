#pragma once
#include "IMateriaServiceProvider.hpp"
#include <messages/admin.pb.h>
#include <Common/ZmqPbChannel.hpp>

namespace materia
{
   
class AdminServiceProvider : public IMateriaServiceProvider
{
public:
   AdminServiceProvider(ZmqPbChannel& channel);
   
   virtual std::string execute(const std::string& input);
   
private:
   std::string getComponentInfo();
   
   admin::AdminService::Stub mService;
   std::string mSenderName;
};
   
}
