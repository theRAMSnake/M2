#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <boost/lexical_cast.hpp>
#include <algorithm>
#include <messages/database.pb.h>
#include "TestServiceProvider.hpp"

#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>

const int GUID_STRING_SIZE = 32 + 4; //4 of '-'

namespace 
{
   void cleanUp()
   {   
      mongocxx::instance instance{}; 
      mongocxx::client client{mongocxx::uri{}};

      client["materia"].drop();
   }
}

BOOST_AUTO_TEST_CASE( Database_Add ) 
{
   cleanUp();
   TestServiceProvider<database::DatabaseService> serviceProvider;
   auto& service = serviceProvider.getService();

   //invalid doc creation
   {
      database::Document request;
      request.set_body("invalid body");
      
      common::UniqueId responce;
      
      service.AddDocument(nullptr, &request, &responce, nullptr);
      BOOST_CHECK(responce.guid().empty());
   }

   common::UniqueId id;
   database::Document doc;
   doc.set_body("{\"somevalue\":5}");
   doc.mutable_header()->set_key("key");
   doc.mutable_header()->set_category("somecat");

   //valid doc creation
   {
      service.AddDocument(nullptr, &doc, &id, nullptr);
      BOOST_CHECK(id.guid() == "key");
   }

   //check if we can get it
   {
      database::Documents result;

      doc.mutable_header()->set_key(id.guid());
      service.GetDocument(nullptr, &doc.header(), &result, nullptr);

      BOOST_CHECK(result.result_size() == 1);
      BOOST_CHECK(result.result(0).header().key() == id.guid());

      std::string result_body = result.result(0).body();
      result_body.erase( std::remove_if( result_body.begin(), result_body.end(), ::isspace ), result_body.end() );
      BOOST_CHECK_EQUAL(doc.body(), result_body);
      BOOST_CHECK(doc.header().category() == result.result(0).header().category());
   }
}

std::vector<std::string> fillSampleDocuments(database::DatabaseService& service)
{
   cleanUp();
   
   std::vector<std::string> result;

   for(int i = 0; i < 10; ++i)
   {
      database::Document doc;
      doc.set_body("{\"doc\":" + boost::lexical_cast<std::string>(i) + ", \"someval\":"
      + boost::lexical_cast<std::string>(i) + "}");
      doc.mutable_header()->set_category("somecat");

      common::UniqueId id;
       
      service.AddDocument(nullptr, &doc, &id, nullptr);
      result.push_back(id.guid());
   }

   return result;
}

BOOST_AUTO_TEST_CASE( Database_Delete ) 
{
   TestServiceProvider<database::DatabaseService> serviceProvider;
   auto& service = serviceProvider.getService();

   std::vector<std::string> guids = fillSampleDocuments(service);

   //delete unexist doc
   {
      database::DocumentHeader head;
      head.set_key("sdfdfdf");
      common::OperationResultMessage result;

      service.DeleteDocument(nullptr, &head, &result, nullptr);
      BOOST_CHECK(!result.success());

      //check that all documents are there
      for(auto x : guids)
      {
         database::DocumentHeader head;
         head.set_key(x);
         head.set_category("somecat");

         database::Documents queryResult;
         service.GetDocument(nullptr, &head, &queryResult, nullptr);

         BOOST_CHECK(queryResult.result_size() == 1);
      }
   }

   //delete exist doc
   {
      const int sampleDocIndex = 2;
      database::DocumentHeader head;
      head.set_key(guids[sampleDocIndex]);
      head.set_category("somecat");
      common::OperationResultMessage result;

      service.DeleteDocument(nullptr, &head, &result, nullptr);
      BOOST_CHECK(result.success());

      //check that all, but deleted documents are there
      for(std::size_t i = 0; i < guids.size(); ++i)
      {
         database::DocumentHeader head;
         head.set_key(guids[i]);
         head.set_category("somecat");

         database::Documents queryResult;
         service.GetDocument(nullptr, &head, &queryResult, nullptr);

         BOOST_CHECK(queryResult.result_size() == (i == sampleDocIndex ? 0 : 1));
      }
   }

   //delete all
   for(auto x : guids)
   {
      database::DocumentHeader head;
      head.set_key(x);
      head.set_category("somecat");

      common::OperationResultMessage result;
      service.DeleteDocument(nullptr, &head, &result, nullptr);      
   }

   //nothing can be obtained anymore
   {
      for(std::size_t i = 0; i < guids.size(); ++i)
      {
         database::DocumentHeader head;
         head.set_key(guids[i]);
         head.set_category("somecat");

         database::Documents queryResult;
         service.GetDocument(nullptr, &head, &queryResult, nullptr);

         BOOST_CHECK(queryResult.result_size() == 0);
      }
   }
}

BOOST_AUTO_TEST_CASE( Database_Get ) 
{
   TestServiceProvider<database::DatabaseService> serviceProvider;
   auto& service = serviceProvider.getService();

   std::vector<std::string> guids = fillSampleDocuments(service);

   //get exist
   for(auto x : guids)
   {
      database::DocumentHeader head;
      head.set_key(x);
      head.set_category("somecat");

      database::Documents queryResult;
      service.GetDocument(nullptr, &head, &queryResult, nullptr);

      BOOST_CHECK(queryResult.result_size() == 1);
   }

   //get non exist
   database::DocumentHeader head;
   head.set_key("asdfklsdf");
   head.set_category("somecat");
   
   database::Documents queryResult;
   service.GetDocument(nullptr, &head, &queryResult, nullptr);

   BOOST_CHECK(queryResult.result_size() == 0);
}

std::string remove_spaces(std::string src)
{
   src.erase(std::remove_if(src.begin(), src.end(), ::isspace), src.end());
   return src;
}

BOOST_AUTO_TEST_CASE( Database_Edit ) 
{
   TestServiceProvider<database::DatabaseService> serviceProvider;
   auto& service = serviceProvider.getService();

   std::vector<std::string> guids = fillSampleDocuments(service);

   //edit non exist
   {
      database::Document doc;
      doc.set_body("{\"doc\":5");
      doc.mutable_header()->set_category("somecat");
      doc.mutable_header()->set_key("dgkljsdg");

      common::OperationResultMessage result;
      service.ModifyDocument(nullptr, &doc, &result, nullptr);

      BOOST_CHECK(!result.success());
   }

   //edit invalid json
   {
      database::Document doc;
      doc.set_body("{\"doc\"5");
      doc.mutable_header()->set_category("somecat");
      doc.mutable_header()->set_key(guids[0]);

      common::OperationResultMessage result;
      service.ModifyDocument(nullptr, &doc, &result, nullptr);

      BOOST_CHECK(!result.success());
   }

   //edit valid
   {
      database::Document doc;
      doc.set_body("{\"doc\":100}");
      doc.mutable_header()->set_category("somecat");
      doc.mutable_header()->set_key(guids[0]);

      common::OperationResultMessage result;
      service.ModifyDocument(nullptr, &doc, &result, nullptr);

      BOOST_CHECK(result.success());
   }

   //check validness of documents
   {
      for(std::size_t i = 0; i < guids.size(); ++i)
      {
         database::DocumentHeader head;
         head.set_key(guids[i]);
         head.set_category("somecat");

         database::Documents queryResult;
         service.GetDocument(nullptr, &head, &queryResult, nullptr);

         BOOST_CHECK(queryResult.result_size() == 1);
         BOOST_CHECK(queryResult.result(0).header().key() == head.key());  
         BOOST_CHECK("somecat" == queryResult.result(0).header().category());

         if(i != 0)
         {
            BOOST_CHECK_EQUAL("{\"doc\":" + boost::lexical_cast<std::string>(i) + ",\"someval\":"
            + boost::lexical_cast<std::string>(i) +
             "}",
             remove_spaces(queryResult.result(0).body()));
         }
         else
         {
            BOOST_CHECK_EQUAL("{\"doc\":100}", remove_spaces(queryResult.result(0).body()));
         }
      }
   }
}

BOOST_AUTO_TEST_CASE( Database_Find_Category )
{
   TestServiceProvider<database::DatabaseService> serviceProvider;
   auto& service = serviceProvider.getService();

   std::vector<std::string> guids = fillSampleDocuments(service);

   {
      database::DocumentQuery query;
      query.set_category("somecat");

      database::Documents queryResult;
      service.SearchDocuments(nullptr, &query, &queryResult, nullptr);

      BOOST_CHECK(queryResult.result_size() == 10);
   }
   {
      database::DocumentQuery query;
      query.set_category("somecat1");

      database::Documents queryResult;
      service.SearchDocuments(nullptr, &query, &queryResult, nullptr);

      BOOST_CHECK(queryResult.result_size() == 0);
   }
} 

BOOST_AUTO_TEST_CASE( Database_Find_Text )
{
   TestServiceProvider<database::DatabaseService> serviceProvider;
   auto& service = serviceProvider.getService();

   std::vector<std::string> guids = fillSampleDocuments(service);

   {
      database::DocumentQuery query;
      auto kval = query.add_query();
      kval->set_key("doc");
      kval->set_value("3");
      kval->set_type(database::QueryElementType::Equals);
      query.set_category("somecat");

      database::Documents queryResult;
      service.SearchDocuments(nullptr, &query, &queryResult, nullptr);

      BOOST_CHECK_EQUAL(queryResult.result_size(), 1);
      BOOST_CHECK_EQUAL("{\"doc\":3,\"someval\":3}", remove_spaces(queryResult.result(0).body()));
   }
   {
      database::DocumentQuery query;
      auto kval = query.add_query();
      kval->set_key("error");
      kval->set_value("");
      kval->set_type(database::QueryElementType::Equals);

      database::Documents queryResult;
      service.SearchDocuments(nullptr, &query, &queryResult, nullptr);

      BOOST_CHECK(queryResult.result_size() == 0);
   }
   {
      database::DocumentQuery query;
      auto kval = query.add_query();
      kval->set_key("doc");
      kval->set_value("11");
      kval->set_type(database::QueryElementType::Equals);
      query.set_category("somecat");

      database::Documents queryResult;
      service.SearchDocuments(nullptr, &query, &queryResult, nullptr);

      BOOST_CHECK(queryResult.result_size() == 0);
   }
   {
      database::DocumentQuery query;
      auto kval = query.add_query();
      kval->set_key("doc");
      kval->set_value("4");
      kval->set_type(database::QueryElementType::Equals);
      kval = query.add_query();
      kval->set_key("someval");
      kval->set_value("4");
      kval->set_type(database::QueryElementType::Equals);
      query.set_category("somecat");

      database::Documents queryResult;
      service.SearchDocuments(nullptr, &query, &queryResult, nullptr);

      BOOST_CHECK_EQUAL(queryResult.result_size(), 1);
      BOOST_CHECK_EQUAL("{\"doc\":4,\"someval\":4}", remove_spaces(queryResult.result(0).body()));
   }
   {
      database::DocumentQuery query;
      auto kval = query.add_query();
      kval->set_key("someval");
      kval->set_value("3");
      kval->set_type(database::QueryElementType::Equals);
      query.set_category("somecat");

      database::Documents queryResult;
      service.SearchDocuments(nullptr, &query, &queryResult, nullptr);

      BOOST_CHECK_EQUAL(queryResult.result_size(), 1);
   }
} 

BOOST_AUTO_TEST_CASE( Database_Query_Less )
{
    TestServiceProvider<database::DatabaseService> serviceProvider;
    auto& service = serviceProvider.getService();
 
    std::vector<std::string> guids = fillSampleDocuments(service);

    {
        database::DocumentQuery query;
        auto kval = query.add_query();
        kval->set_key("someval");
        kval->set_value("5");
        kval->set_type(database::QueryElementType::Less);
        query.set_category("somecat");
  
        database::Documents queryResult;
        service.SearchDocuments(nullptr, &query, &queryResult, nullptr);
  
        BOOST_CHECK_EQUAL(queryResult.result_size(), 5);
    }
    {
        database::DocumentQuery query;
        auto kval = query.add_query();
        kval->set_key("someval");
        kval->set_value("1");
        kval->set_type(database::QueryElementType::Less);
        query.set_category("somecat");
  
        database::Documents queryResult;
        service.SearchDocuments(nullptr, &query, &queryResult, nullptr);
  
        BOOST_CHECK_EQUAL(queryResult.result_size(), 1);
    }
}

BOOST_AUTO_TEST_CASE( Database_Query_Greater )
{
    TestServiceProvider<database::DatabaseService> serviceProvider;
    auto& service = serviceProvider.getService();
 
    std::vector<std::string> guids = fillSampleDocuments(service);

    {
        database::DocumentQuery query;
        auto kval = query.add_query();
        kval->set_key("someval");
        kval->set_value("1");
        kval->set_type(database::QueryElementType::Greater);
        query.set_category("somecat");
  
        database::Documents queryResult;
        service.SearchDocuments(nullptr, &query, &queryResult, nullptr);
  
        BOOST_CHECK_EQUAL(queryResult.result_size(), 8);
    }
    {
        database::DocumentQuery query;
        auto kval = query.add_query();
        kval->set_key("someval");
        kval->set_value("5");
        kval->set_type(database::QueryElementType::Greater);
        query.set_category("somecat");
  
        database::Documents queryResult;
        service.SearchDocuments(nullptr, &query, &queryResult, nullptr);
  
        BOOST_CHECK_EQUAL(queryResult.result_size(), 4);
    }
}

BOOST_AUTO_TEST_CASE( Database_Query_Between )
{
    TestServiceProvider<database::DatabaseService> serviceProvider;
    auto& service = serviceProvider.getService();
 
    std::vector<std::string> guids = fillSampleDocuments(service);

    {
        database::DocumentQuery query;
        auto kval = query.add_query();
        kval->set_key("someval");
        kval->set_value("1");
        kval->set_value2("5");
        kval->set_type(database::QueryElementType::Between);
        query.set_category("somecat");
  
        database::Documents queryResult;
        service.SearchDocuments(nullptr, &query, &queryResult, nullptr);
  
        BOOST_CHECK_EQUAL(queryResult.result_size(), 3);
    }
    {
        database::DocumentQuery query;
        auto kval = query.add_query();
        kval->set_key("someval");
        kval->set_value("5");
        kval->set_value2("1");
        kval->set_type(database::QueryElementType::Between);
        query.set_category("somecat");
  
        database::Documents queryResult;
        service.SearchDocuments(nullptr, &query, &queryResult, nullptr);
  
        BOOST_CHECK_EQUAL(queryResult.result_size(), 0);
    }
}

BOOST_AUTO_TEST_CASE( Database_Fetch ) 
{
   TestServiceProvider<database::DatabaseService> serviceProvider;
   auto& service = serviceProvider.getService();

   std::vector<std::string> guids = fillSampleDocuments(service);

   common::UniqueId id;
   database::Document doc;
   doc.set_body("{\"somevalue\":5}");
   doc.mutable_header()->set_key("key");
   doc.mutable_header()->set_category("someothercat");

   //valid doc creation
   {
      service.AddDocument(nullptr, &doc, &id, nullptr);
   }

   {
      common::EmptyMessage emptyMsg;

      database::Documents fetchResult;
      service.Fetch(nullptr, &emptyMsg, &fetchResult, nullptr);

      BOOST_CHECK_EQUAL(fetchResult.result_size(), 11);

      for(auto x : fetchResult.result())
      {
         bool keyFound = std::find_if(guids.begin(), guids.end(), [&](auto y)->bool{return y == x.header().key();})
            != guids.end();

         BOOST_CHECK(keyFound || x.header().key() == id.guid());
      }
   }
}