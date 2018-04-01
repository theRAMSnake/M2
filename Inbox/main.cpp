#include <iostream>
#include <fstream>
#include <Common/InterprocessService.hpp>
#include <Common/PortLayout.hpp>
#include <Client/MateriaClient.hpp>
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
   
boost::uuids::random_generator generator;
boost::filesystem::path g_dir("inbox_service_data");

std::string remove_spaces(std::string src)
{
   src.erase(std::remove_if(src.begin(), src.end(), ::isspace), src.end());
   return src;
}

std::string from_simple_json(const std::string& json)
{
   boost::property_tree::ptree pt;
   std::istringstream is (json);
   read_json (is, pt);

   return pt.get<std::string> ("content");

   /*std::string trimmedJson = remove_spaces(json);
   boost::replace_all(trimmedJson, "\\\"", "\"");
   std::size_t pos = trimmedJson.find(":\"") + 2;
   return trimmedJson.substr(pos, (trimmedJson.find("}") - 1) - pos);*/
}

std::string to_simple_json(const std::string& content)
{
   /*std::string newContent = content;
   boost::replace_all(newContent, "\"", "\\\"");
   return "{\"content\":\"" + newContent + "\"}";*/

   boost::property_tree::ptree pt;

   pt.put ("content", content);

   std::ostringstream buf; 
   write_json (buf, pt, false);
   return buf.str();
}

class InboxServiceImpl : public inbox::InboxService
{
public:
   InboxServiceImpl()
   : mClient("InboxService")
   , mDbProxy(mClient.getDatabase())
   {
      mDbProxy.setCategory(mCategory);
   }

   virtual void GetInbox(::google::protobuf::RpcController* controller,
                       const ::common::EmptyMessage* request,
                       ::inbox::InboxItems* response,
                       ::google::protobuf::Closure* done)
   {
      for(auto x : mDbProxy.getDocuments())
      {
         auto item = response->add_items();
         *item->mutable_id() = x.id.toProtoId();
         item->set_text(from_simple_json(x.body));
      }
   }

   virtual void DeleteItem(::google::protobuf::RpcController* controller,
                       const ::common::UniqueId* request,
                       ::common::OperationResultMessage* response,
                       ::google::protobuf::Closure* done)
   {
      response->set_success(mDbProxy.deleteDocument(*request));
   }

   virtual void EditItem(::google::protobuf::RpcController* controller,
                       const ::inbox::InboxItemInfo* request,
                       ::common::OperationResultMessage* response,
                       ::google::protobuf::Closure* done)
   {
      materia::Document doc { request->id(), to_simple_json(request->text()) };
      response->set_success(mDbProxy.replaceDocument(doc));
   }

   virtual void AddItem(::google::protobuf::RpcController* controller,
                       const ::inbox::InboxItemInfo* request,
                       ::common::UniqueId* response,
                       ::google::protobuf::Closure* done)
   {
      std::string id = to_string(generator());

      materia::Document doc { id, to_simple_json(request->text()) };
      *response = mDbProxy.insertDocument(doc, materia::IdMode::Provided).toProtoId();
   }

private:
   materia::MateriaClient mClient;
   materia::Database& mDbProxy;
   const std::string mCategory = "INBOX";
};

}

int main(int argc, char *argv[])
{
   materia::InboxServiceImpl serviceImpl;
   materia::InterprocessService<materia::InboxServiceImpl> service(serviceImpl);
   
   service.provideAt("*:" + gInboxPort, "InboxService");
   
   return 0;
}
