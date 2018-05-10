#include "Admin.hpp"

namespace materia
{

Admin::Admin(materia::ZmqPbChannel& channel)
: mProxy(channel)
{

}

std::vector<ServiceStatus> Admin::getServiceStatus()
{
   common::EmptyMessage emptyMsg;
   
   admin::ComponentInfoResponce responce;
   mProxy.getService().GetComponentInfo(nullptr, &emptyMsg, &responce, nullptr);

   std::vector<ServiceStatus> result;

   for(auto x : responce.componentinfos())
   {
      result.push_back({x.name(), x.status()});
   }

   return result;
}

}