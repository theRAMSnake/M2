#include <iostream>
#include <fstream>
#include <Common/InterprocessService.hpp>
#include <Common/PortLayout.hpp>
#include <messages/inbox.pb.h>
#include <boost/filesystem.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace materia
{
   
boost::uuids::random_generator generator;
boost::filesystem::path g_dir("inbox_service_data");

class InboxServiceImpl : public inbox::InboxService
{
public:
   InboxServiceImpl()
   {
      boost::filesystem::create_directory(g_dir);
   }

   virtual void GetInbox(::google::protobuf::RpcController* controller,
                       const ::common::EmptyMessage* request,
                       ::inbox::InboxItems* response,
                       ::google::protobuf::Closure* done)
   {
      for (boost::filesystem::directory_iterator iter(g_dir); iter != boost::filesystem::directory_iterator(); ++iter)
      {
         std::string content;

         std::ifstream file(iter->path().string());

         content = std::string((std::istreambuf_iterator<char>(file)),
                 std::istreambuf_iterator<char>());

         file.close();

         auto item = response->add_items();
         item->mutable_id()->set_guid(iter->path().filename().string());
         item->set_text(content);
      }
   }

   virtual void DeleteItem(::google::protobuf::RpcController* controller,
                       const ::common::UniqueId* request,
                       ::common::OperationResultMessage* response,
                       ::google::protobuf::Closure* done)
   {
      auto path = g_dir / request->guid();
      if(boost::filesystem::exists(path))
      {
         response->set_success(true);
         boost::filesystem::remove(path);
      }
      else
      {
         response->set_success(false);
      }
   }

   virtual void EditItem(::google::protobuf::RpcController* controller,
                       const ::inbox::InboxItemInfo* request,
                       ::common::OperationResultMessage* response,
                       ::google::protobuf::Closure* done)
   {
      auto path = g_dir / request->id().guid();
      if(boost::filesystem::exists(path))
      {
         response->set_success(true);
         saveItem(request->id().guid(), request->text());
      }
      else
      {
         response->set_success(false);
      }
   }

   virtual void AddItem(::google::protobuf::RpcController* controller,
                       const ::inbox::InboxItemInfo* request,
                       ::common::UniqueId* response,
                       ::google::protobuf::Closure* done)
   {
      std::string id = to_string(generator());
      saveItem(id, request->text());

      response->set_guid(id);
   }

private:
   void saveItem(const std::string& fileName, const std::string& text)
   {
      std::ofstream f((g_dir / fileName).string());
      f << text;
      f.close();
   }
};

}

int main(int argc, char *argv[])
{
   materia::InboxServiceImpl serviceImpl;
   materia::InterprocessService<materia::InboxServiceImpl> service(serviceImpl);
   
   service.provideAt("*:" + gInboxPort, "InboxService");
   
   return 0;
}
