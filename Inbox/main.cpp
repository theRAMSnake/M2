#include <iostream>
#include <fstream>
#include <Common/InterprocessService.hpp>
#include <Common/MateriaServiceProxy.hpp>
#include <Common/PortLayout.hpp>
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
   {
      mService.reset(new MateriaServiceProxy<database::DatabaseService>("InboxService"));
      mDatabase = &mService->getService();
   }

   virtual void GetInbox(::google::protobuf::RpcController* controller,
                       const ::common::EmptyMessage* request,
                       ::inbox::InboxItems* response,
                       ::google::protobuf::Closure* done)
   {
      ///Old style impl
      /*for (boost::filesystem::directory_iterator iter(g_dir); iter != boost::filesystem::directory_iterator(); ++iter)
      {
         std::string content;

         std::ifstream file(iter->path().string());

         content = std::string((std::istreambuf_iterator<char>(file)),
                 std::istreambuf_iterator<char>());

         file.close();

         auto item = response->add_items();
         item->mutable_id()->set_guid(iter->path().filename().string());
         item->set_text(content);
      }*/
      database::DocumentQuery query;
      query.set_category(mCategory);

      database::Documents result;
      mDatabase->SearchDocuments(nullptr, &query, &result, nullptr);

      for(auto x : result.result())
      {
         auto item = response->add_items();
         item->mutable_id()->set_guid(x.header().key());
         item->set_text(from_simple_json(x.body()));
      }
   }

   virtual void DeleteItem(::google::protobuf::RpcController* controller,
                       const ::common::UniqueId* request,
                       ::common::OperationResultMessage* response,
                       ::google::protobuf::Closure* done)
   {
      database::DocumentHeader head;
      head.set_key(request->guid());
      head.set_category(mCategory);
      common::OperationResultMessage result;

      mDatabase->DeleteDocument(nullptr, &head, &result, nullptr);
      response->set_success(result.success());
   }

   virtual void EditItem(::google::protobuf::RpcController* controller,
                       const ::inbox::InboxItemInfo* request,
                       ::common::OperationResultMessage* response,
                       ::google::protobuf::Closure* done)
   {
      database::Document doc;
      doc.set_body(to_simple_json(request->text()));
      doc.mutable_header()->set_category(mCategory);
      doc.mutable_header()->set_key(request->id().guid());

      common::OperationResultMessage result;
      mDatabase->ModifyDocument(nullptr, &doc, &result, nullptr);

      response->set_success(result.success());
   }

   virtual void AddItem(::google::protobuf::RpcController* controller,
                       const ::inbox::InboxItemInfo* request,
                       ::common::UniqueId* response,
                       ::google::protobuf::Closure* done)
   {
      std::string id = to_string(generator());

      common::UniqueId result;
      database::Document doc;
      doc.set_body(to_simple_json(request->text()));
      doc.mutable_header()->set_key(id);
      doc.mutable_header()->set_category(mCategory);
   
      mDatabase->AddDocument(nullptr, &doc, &result, nullptr);
      response->set_guid(result.guid());
   }

private:
   std::unique_ptr<MateriaServiceProxy<database::DatabaseService>> mService;
   database::DatabaseService_Stub* mDatabase;
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
