#pragma once
#include <messages/admin.pb.h>

namespace materia
{
   
class IComponentInfoProvider
{
public:
   struct Info
   {
      bool status = false;
      std::string name;
   };
   
   virtual std::vector<Info> getComponentInfos() const = 0;
   
   virtual ~IComponentInfoProvider(){}
};
   
class AdminServiceImpl : public admin::AdminService
{
public:
   AdminServiceImpl(const IComponentInfoProvider& componentInfoProvider);
   
   virtual void GetComponentInfo(::google::protobuf::RpcController* controller,
                       const ::common::EmptyMessage* request,
                       ::admin::ComponentInfoResponce* response,
                       ::google::protobuf::Closure* done);
   
private:
   const IComponentInfoProvider& mComponentInfoProvider;
};
   
}
