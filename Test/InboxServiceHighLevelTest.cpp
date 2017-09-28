#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <messages/inbox.pb.h>
#include "TestServiceProvider.hpp"
#include <boost/filesystem.hpp>

#include <mongocxx/instance.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>

namespace 
{
   void cleanUp()
   {   
      boost::filesystem::remove_all("inbox_service_data");
      boost::filesystem::create_directory("inbox_service_data");

      mongocxx::instance instance{}; 
      mongocxx::client client{mongocxx::uri{}};

      client["materia"].drop();
   }
}

bool isItemsConsistent(inbox::InboxService& service)
{
   common::EmptyMessage emptyMsg;
   inbox::InboxItems responce;
   
   service.GetInbox(nullptr, &emptyMsg, &responce, nullptr);
   
   return 0 == responce.items_size();
}

const int GUID_STRING_SIZE = 32 + 4; //4 of '-'

BOOST_AUTO_TEST_CASE( Inbox_AddDeleteInbox ) 
{
   cleanUp();

   TestServiceProvider<inbox::InboxService> serviceProvider;
   auto& service = serviceProvider.getService();
   
   inbox::InboxItemInfo request;
   request.set_text("text");
   
   common::UniqueId responce;
   
   service.AddItem(nullptr, &request, &responce, nullptr);
   BOOST_CHECK(responce.guid().size() == GUID_STRING_SIZE);
   
   {
      common::EmptyMessage emptyMsg;
      inbox::InboxItems responce;
      
      service.GetInbox(nullptr, &emptyMsg, &responce, nullptr);
      
      BOOST_CHECK_EQUAL(1, responce.items_size());

      //printf("%s", responce.items().begin()->text().c_str());
      
      auto pos = std::find_if(responce.items().begin(), responce.items().end(), 
         [](auto x){return x.text() == "text";});
      BOOST_REQUIRE(pos != responce.items().end());
      
      {
         common::UniqueId request;
         request.set_guid(pos->id().guid());
         
         common::OperationResultMessage opResult;
         service.DeleteItem(nullptr, &request, &opResult, nullptr);
         
         BOOST_CHECK(opResult.success());
         BOOST_CHECK(isItemsConsistent(service));
      }
   }
}

BOOST_AUTO_TEST_CASE( Inbox_DeleteWrongInbox ) 
{
   cleanUp();

   TestServiceProvider<inbox::InboxService> serviceProvider;
   auto& service = serviceProvider.getService();
   
   common::UniqueId request;
   request.set_guid("50");
   
   common::OperationResultMessage opResult;
   service.DeleteItem(nullptr, &request, &opResult, nullptr);
   
   BOOST_CHECK(!opResult.success());
   BOOST_CHECK(isItemsConsistent(service));
}

BOOST_AUTO_TEST_CASE( Inbox_EditWrongInbox ) 
{
   cleanUp();

   TestServiceProvider<inbox::InboxService> serviceProvider;
   auto& service = serviceProvider.getService();
   
   inbox::InboxItemInfo request;
   request.set_text("text");
   request.mutable_id()->set_guid("50");
   
   common::OperationResultMessage opResult;
   
   service.EditItem(nullptr, &request, &opResult, nullptr);
   
   BOOST_CHECK(!opResult.success());
   BOOST_CHECK(isItemsConsistent(service));
}

BOOST_AUTO_TEST_CASE( Inbox_EditInbox ) 
{
   cleanUp(); 

   TestServiceProvider<inbox::InboxService> serviceProvider;
   auto& service = serviceProvider.getService();
   
   inbox::InboxItemInfo request;
   request.set_text("text");
   
   common::UniqueId responce;
   
   service.AddItem(nullptr, &request, &responce, nullptr);
   
   BOOST_CHECK(responce.guid().size() == GUID_STRING_SIZE);
   
   request.mutable_id()->set_guid(responce.guid());
   request.set_text("other_text");
   
   common::OperationResultMessage opResult;
   service.EditItem(nullptr, &request, &opResult, nullptr);
   
   BOOST_CHECK(opResult.success());
   
   {
      common::EmptyMessage emptyMsg;
      inbox::InboxItems responce;
      
      service.GetInbox(nullptr, &emptyMsg, &responce, nullptr);
      
      BOOST_CHECK_EQUAL(1, responce.items_size());
      
      auto pos = std::find_if(responce.items().begin(), responce.items().end(), 
         [](auto x){return x.text() == "other_text";});
      BOOST_REQUIRE(pos != responce.items().end());
      BOOST_CHECK(pos->id().guid() == request.id().guid());
      
      {
         common::UniqueId request;
         request.set_guid(pos->id().guid());
         
         common::OperationResultMessage opResult;
         service.DeleteItem(nullptr, &request, &opResult, nullptr);
         
         BOOST_CHECK(opResult.success());
         BOOST_CHECK(isItemsConsistent(service));
      }
   }
}