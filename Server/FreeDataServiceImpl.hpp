#pragma once

#include <messages/freedata.pb.h>
#include <Core/IFreeData.hpp>
#include "ProtoConvertion.hpp"

namespace materia
{

class FreeDataServiceImpl : public freedata::FreeDataService
{
public:
   FreeDataServiceImpl(ICore& core)
   : mFd(core.getFreeData())
   {
   }

   virtual void Get(::google::protobuf::RpcController* controller,
                       const ::common::EmptyMessage* request,
                       ::freedata::FreeDataBlocks* response,
                       ::google::protobuf::Closure* done)
   {
      for(auto x : mFd.get())
      {
         auto item = response->add_items();
         item->set_name(x.name);
         item->set_value(x.value);
      }
   }

   virtual void Remove(::google::protobuf::RpcController* controller,
                        const ::common::StringMessage* request,
                        ::common::OperationResultMessage* response,
                        ::google::protobuf::Closure* done)
   {
      mFd.remove(request->content());
      response->set_success(true);
   }

   virtual void Set(::google::protobuf::RpcController* controller,
                        const ::freedata::FreeDataBlock* request,
                        ::common::OperationResultMessage* response,
                        ::google::protobuf::Closure* done)
   {
      mFd.set({request->name(), request->value()});
      response->set_success(true);
   }

   virtual void Increment(::google::protobuf::RpcController* controller,
                        const ::freedata::FreeDataBlock* request,
                        ::common::OperationResultMessage* response,
                        ::google::protobuf::Closure* done)
   {
      mFd.increment(request->name(), request->value());
      response->set_success(true);
   }

   virtual void CheckExpression(::google::protobuf::RpcController* controller,
                       const ::common::StringMessage* request,
                       ::common::OperationResultMessage* response,
                       ::google::protobuf::Closure* done)
   {
      response->set_success(mFd.checkExpression(request->content()));
   }

private:
   materia::IFreeData& mFd;
};

}