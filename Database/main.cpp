#include <iostream>
#include <fstream>
#include <Common/InterprocessService.hpp>
#include <Common/PortLayout.hpp>
#include <messages/database.pb.h>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>

using namespace bsoncxx::builder::stream;

namespace materia
{

boost::uuids::random_generator generator;

class DatabaseServiceImpl : public database::DatabaseService
{
public:
   DatabaseServiceImpl(mongocxx::database&& db)
   : mDb(std::move(db))
   {

   }

   void GetDocument(::google::protobuf::RpcController* controller,
                    const::database::DocumentHeader* request,
                    ::database::Documents* response,
                    ::google::protobuf::Closure* done)
   {
      if(request->category().empty())
      {
         return;
      }

      bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
         mDb[request->category()].find_one(document{} << "key" << request->key() << finalize);

      if(maybe_result) 
      {
         auto doc = response->add_result();
         doc->set_body(bsoncxx::to_json(maybe_result->view()["body"].get_document().view()));
         doc->mutable_header()->set_key(request->key());
         doc->mutable_header()->set_category(request->category());
      }
   }

   std::string composeMongoExpression(const database::QueryElementType type, const std::string& val, const std::string& val2)
   {
      switch(type)
      {
      case database::QueryElementType::Equals:
         return val;

      case database::QueryElementType::Less:
         return "{\"$lt\": " + val + " }";

      case database::QueryElementType::Greater:
         return "{\"$gt\": " + val + " }";

      case database::QueryElementType::Between:
         return "{\"$gt\": " + val + " , \"$lt\": " + val2 + " }";

      default:
         return "";
      }
   }

   void SearchDocuments(::google::protobuf::RpcController* controller,
                        const ::database::DocumentQuery* request,
                        ::database::Documents* response,
                        ::google::protobuf::Closure* done)
   {
      //check input values
      if(request->category().empty())
      {
         return;
      }
      
      std::string json_filter = "{";
      for(auto kval : request->query())
      {
          if(json_filter.size() != 1)
          {
              json_filter += ", ";
          }
          json_filter += "\"body." + kval.key() + "\":" + composeMongoExpression(kval.type(), kval.value(), kval.value2());
      }
      json_filter += "}";

       try
       {
         printf("\nJSON FILTER: %s\n", json_filter.c_str());
         bsoncxx::document::value body = bsoncxx::from_json(json_filter);
         
         for(auto x : mDb[request->category()].find(bsoncxx::types::b_document{body.view()}.view()))
         {
               auto doc = response->add_result();
               doc->set_body(bsoncxx::to_json(x["body"].get_document().view()));
               doc->mutable_header()->set_key(x["key"].get_utf8().value.to_string());
               doc->mutable_header()->set_category(request->category());
         }
        }
       catch(...)
       {
          //Compose with BSON instead
          printf("\nGOTCHA\n");
       }
   }

   void AddDocument(::google::protobuf::RpcController* controller,
                        const ::database::Document* request,
                        ::common::UniqueId* response,
                        ::google::protobuf::Closure* done)
   {
      if(checkDocument(*request))
      {
          try
          {
              std::string key = request->header().key().empty() ? to_string(generator()) : request->header().key();
              bsoncxx::document::value body = bsoncxx::from_json(request->body());
              mDb[request->header().category()].insert_one(
                document{} << "key" << key
                << "body" << bsoncxx::types::b_document{body.view()}
                << finalize
                );

              response->set_guid(key);
          }
          catch(...)
          {

          }
      }
   }

   void ModifyDocument(::google::protobuf::RpcController* controller,
                        const ::database::Document* request,
                        ::common::OperationResultMessage* response,
                        ::google::protobuf::Closure* done)
   {
      if(!checkDocument(*request))
      {
         response->set_success(false);
         return;
      }

      try
      {
            bsoncxx::document::value body = bsoncxx::from_json(request->body());
            bsoncxx::document::value filter = document{} << "key" << request->header().key() << finalize;
            
            auto result = mDb[request->header().category()].replace_one(
                filter.view(),
                document{} << "key" << request->header().key()
                << "body" << bsoncxx::types::b_document{body.view()} 
                << finalize
                );

            if(result && result->modified_count() == 1)
            {
                response->set_success(true);
            }
            else
            {
                response->set_success(false);
            }
      }
      catch(...)
      {
          response->set_success(false);
      }      
   }

   void DeleteDocument(::google::protobuf::RpcController* controller,
                        const ::database::DocumentHeader* request,
                        ::common::OperationResultMessage* response,
                        ::google::protobuf::Closure* done)
   {
      if(request->category().empty())
      {
         response->set_success(false);
         return;
      }

      bsoncxx::document::value filter = document{} << "key" << request->key() << finalize;
      auto result = mDb[request->category()].find_one_and_delete(filter.view());

      if(result)
      {
         response->set_success(true);
         return;
      }

      response->set_success(false);
   }

   virtual void Fetch(::google::protobuf::RpcController* controller,
                        const ::common::EmptyMessage* request,
                        ::database::Documents* response,
                        ::google::protobuf::Closure* done)
   {
      for(auto x : mDb.list_collections())
      {
         std::string name = x["name"].get_utf8().value.to_string();
         database::DocumentQuery query;
         query.set_category(name);

         SearchDocuments(nullptr, &query, response, nullptr);
      }
   }

private:

   bool checkDocument(const ::database::Document& doc)
   {
      return !doc.header().category().empty();
   }

   mongocxx::database mDb;
};

}

int main(int argc, char *argv[])
{
   system("systemctl start mongodb");

   mongocxx::instance instance{}; 
   mongocxx::client client{mongocxx::uri{}};

   materia::DatabaseServiceImpl serviceImpl(client["materia"]);
   materia::InterprocessService<materia::DatabaseServiceImpl> service(serviceImpl);
   
   service.provideAt("*:" + gDatabasePort, "DatabaseService");
   
   return 0;
}