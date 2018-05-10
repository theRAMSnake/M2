#include <iostream>
#include <fstream>
#include <Common/InterprocessService.hpp>
#include <Common/PortLayout.hpp>
#include <Client/MateriaClient.hpp>
#include <Client/IContainer.hpp>
#include <Client/private/ProtoConverter.hpp>
#include <messages/inbox.pb.h>
#include <messages/database.pb.h>
#include <boost/filesystem.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace materia
{

class InboxServiceImpl : public inbox::InboxService
{
public:
   InboxServiceImpl()
   : mClient("InboxService")
   , mCnProxy(mClient.getContainer())
   {
      mCnProxy.addContainer({mContainerName, false});
   }

   virtual void GetInbox(::google::protobuf::RpcController* controller,
                       const ::common::EmptyMessage* request,
                       ::inbox::InboxItems* response,
                       ::google::protobuf::Closure* done)
   {
      for(auto x : mCnProxy.getItems(mContainerName))
      {
         auto item = response->add_items();
         *item->mutable_id() = toProto(x.id);
         item->set_text(x.content);
      }
   }

   virtual void DeleteItem(::google::protobuf::RpcController* controller,
                       const ::common::UniqueId* request,
                       ::common::OperationResultMessage* response,
                       ::google::protobuf::Closure* done)
   {
      response->set_success(mCnProxy.deleteItems(mContainerName, {fromProto(*request)}));
   }

   virtual void EditItem(::google::protobuf::RpcController* controller,
                       const ::inbox::InboxItemInfo* request,
                       ::common::OperationResultMessage* response,
                       ::google::protobuf::Closure* done)
   {
      materia::ContainerItem item { fromProto(request->id()), request->text() };
      response->set_success(mCnProxy.replaceItems(mContainerName, {item}));
   }

   virtual void AddItem(::google::protobuf::RpcController* controller,
                       const ::inbox::InboxItemInfo* request,
                       ::common::UniqueId* response,
                       ::google::protobuf::Closure* done)
   {
      materia::ContainerItem item { materia::Id::Invalid, request->text() };
      auto ids = mCnProxy.insertItems(mContainerName, {item});
      if(!ids.empty())
      {
         *response = toProto(*ids.begin());
      }
   }

private:
   materia::MateriaClient mClient;
   materia::IContainer& mCnProxy;
   const std::string mContainerName = "inbox";
};

}

int main(int argc, char *argv[])
{
   materia::InboxServiceImpl serviceImpl;
   materia::InterprocessService<materia::InboxServiceImpl> service(serviceImpl);
   
   service.provideAt("*:" + gInboxPort, "InboxService");
   
   return 0;
}
