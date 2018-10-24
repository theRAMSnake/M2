#pragma once

#include <messages/inbox.pb.h>
#include <Core/IInbox.hpp>
#include "ProtoConvertion.hpp"

namespace materia
{

class InboxServiceImpl : public inbox::InboxService
{
public:
   InboxServiceImpl(ICore& core)
   : mInbox(core.getInbox())
   {
   }

   virtual void GetInbox(::google::protobuf::RpcController* controller,
                       const ::common::EmptyMessage* request,
                       ::inbox::InboxItems* response,
                       ::google::protobuf::Closure* done)
   {
      for(auto x : mInbox.get())
      {
         auto item = response->add_items();
         *item->mutable_id() = toProto(x.id);
         item->set_text(x.text);
      }
   }

   virtual void DeleteItem(::google::protobuf::RpcController* controller,
                       const ::common::UniqueId* request,
                       ::common::OperationResultMessage* response,
                       ::google::protobuf::Closure* done)
   {
      mInbox.remove(fromProto(*request));
      response->set_success(true);
   }

   virtual void EditItem(::google::protobuf::RpcController* controller,
                       const ::inbox::InboxItemInfo* request,
                       ::common::OperationResultMessage* response,
                       ::google::protobuf::Closure* done)
   {
      mInbox.replace({fromProto(request->id()), request->text()});
      response->set_success(true);
   }

   virtual void AddItem(::google::protobuf::RpcController* controller,
                       const ::inbox::InboxItemInfo* request,
                       ::common::UniqueId* response,
                       ::google::protobuf::Closure* done)
   {
      *response = toProto(mInbox.add({ materia::Id::Invalid, request->text() }));
   }

private:
   materia::IInbox& mInbox;
};

}