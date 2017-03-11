#include "AdminServiceImpl.hpp"

namespace materia
{
   
AdminServiceImpl::AdminServiceImpl(const IComponentInfoProvider& componentInfoProvider)
: mComponentInfoProvider(componentInfoProvider)
{
   
}
   
void AdminServiceImpl::GetComponentInfo(::google::protobuf::RpcController* controller,
                       const ::common::EmptyMessage* request,
                       ::admin::ComponentInfoResponce* response,
                       ::google::protobuf::Closure* done)
{ 
   auto cmpInfos = mComponentInfoProvider.getComponentInfos();
   for(auto x : cmpInfos)
   {
      auto cmpInfo = response->add_componentinfos();
      cmpInfo->set_status(x.status);
      cmpInfo->set_name(x.name);
   }
}
   
}
