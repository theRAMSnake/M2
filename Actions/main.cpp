#include <iostream>
#include <fstream>
#include <Common/InterprocessService.hpp>
#include <Common/PortLayout.hpp>
#include <messages/actions.pb.h>
#include <boost/filesystem.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <Common/MateriaServiceProxy.hpp>
#include <messages/database.pb.h>

namespace materia
{

boost::uuids::random_generator generator;
boost::filesystem::path g_dir("actions_service_data");

void from_json(const std::string& json, actions::ActionInfo& result)
{
   boost::property_tree::ptree pt;
   std::istringstream is (json);
   read_json (is, pt);
   
   result.mutable_id()->set_guid(pt.get<std::string> ("id"));
   result.mutable_parentid()->set_guid(pt.get<std::string> ("parent_id"));
   result.set_title(pt.get<std::string> ("title"));
   result.set_description(pt.get<std::string> ("description"));
   result.set_type(static_cast<actions::ActionType> (pt.get<int> ("type")));
}

std::string to_json(const actions::ActionInfo& from)
{
   boost::property_tree::ptree pt;

   pt.put ("id", from.id().guid());
   pt.put ("parent_id", from.parentid().guid());
   pt.put ("title", from.title());
   pt.put ("description", from.description());
   pt.put ("type", from.type());

   std::ostringstream buf; 
   write_json (buf, pt, false);
   return buf.str();
}

std::string make_string_query_value(const std::string& from)
{
   return "\"" + from + "\"";
}

class ActionsServiceImpl : public actions::ActionsService
{
public:
   ActionsServiceImpl()
   {
      mService.reset(new MateriaServiceProxy<database::DatabaseService>("InboxService"));
      mDatabase = &mService->getService();
   }

   void GetChildren(::google::protobuf::RpcController* controller,
                       const ::common::UniqueId* request,
                       ::actions::ActionsList* response,
                       ::google::protobuf::Closure* done)
   {
      database::DocumentQuery query;
      query.set_category(mCategory);
      auto kvl = query.add_query();
      kvl->set_key("parent_id");
      kvl->set_value(make_string_query_value(request->guid()));

      database::Documents result;
      mDatabase->SearchDocuments(nullptr, &query, &result, nullptr);

      for(auto x : result.result())
      {
         auto item = response->add_list();
         from_json(x.body(), *item);
      }
   }

   void GetParentlessElements(::google::protobuf::RpcController* controller,
                       const ::common::EmptyMessage* request,
                       ::actions::ActionsList* response,
                       ::google::protobuf::Closure* done)
   {
      database::DocumentQuery query;
      query.set_category(mCategory);
      auto kvl = query.add_query();
      kvl->set_key("parent_id");
      kvl->set_value(make_string_query_value(""));

      database::Documents result;
      mDatabase->SearchDocuments(nullptr, &query, &result, nullptr);

      for(auto x : result.result())
      {
         auto item = response->add_list();
         from_json(x.body(), *item);
      }
   }

   void AddElement(::google::protobuf::RpcController* controller,
                       const ::actions::ActionInfo* request,
                       ::common::UniqueId* response,
                       ::google::protobuf::Closure* done)
   {
      if(request->parentid().guid().empty() || is_item_exist(request->parentid().guid()))
      {
         std::string id = to_string(generator());
         
         actions::ActionInfo newItem(*request);
         newItem.mutable_id()->set_guid(id);

         common::UniqueId result;
         database::Document doc;
         doc.set_body(to_json(newItem));
         doc.mutable_header()->set_key(id);
         doc.mutable_header()->set_category(mCategory);
      
         mDatabase->AddDocument(nullptr, &doc, &result, nullptr);
         response->set_guid(result.guid());
      }
   }

   bool is_item_exist(const std::string& guid)
   {
      database::DocumentHeader head;
      head.set_category(mCategory);
      head.set_key(guid);

      database::Documents result;
      mDatabase->GetDocument(nullptr, &head, &result, nullptr);

      return result.result_size() > 0;
   }

   void DeleteElement(::google::protobuf::RpcController* controller,
                       const ::common::UniqueId* request,
                       ::common::OperationResultMessage* response,
                       ::google::protobuf::Closure* done)
   {
      auto id = request->guid();

      database::DocumentHeader head;
      head.set_key(id);
      head.set_category(mCategory);
      common::OperationResultMessage result;

      mDatabase->DeleteDocument(nullptr, &head, &result, nullptr);
      if(result.success())
      {
         actions::ActionsList children;
         GetChildren(0, request, &children, 0);
         for(int i = 0; i < children.list_size(); ++i)
         {
            common::OperationResultMessage dummy;
            DeleteElement(0, &children.list(i).id(), &dummy, 0);
         }

         response->set_success(true);
      }
      else
      {
         response->set_success(false);
      }
   }

   void EditElement(::google::protobuf::RpcController* controller,
                       const ::actions::ActionInfo* request,
                       ::common::OperationResultMessage* response,
                       ::google::protobuf::Closure* done)
   {
      auto id = request->id().guid();
      auto parent_id = request->parentid().guid();

      if(id != parent_id)
      {
         if(parent_id.empty() || is_item_exist(parent_id))
         {
            database::Document doc;
            doc.set_body(to_json(*request));
            doc.mutable_header()->set_category(mCategory);
            doc.mutable_header()->set_key(id);
      
            common::OperationResultMessage result;
            mDatabase->ModifyDocument(nullptr, &doc, &result, nullptr);
      
            response->set_success(result.success());
            return;
         }
      }

      response->set_success(false);
   }

private:
   std::unique_ptr<MateriaServiceProxy<database::DatabaseService>> mService;
   database::DatabaseService_Stub* mDatabase;
   const std::string mCategory = "ACTIONS";
};

}

int main(int argc, char *argv[])
{
   materia::ActionsServiceImpl serviceImpl;
   materia::InterprocessService<materia::ActionsServiceImpl> service(serviceImpl);
   
   service.provideAt("*:" + gActionsPort, "ActionsService");
   
   return 0;
}