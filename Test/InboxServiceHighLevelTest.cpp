#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <messages/inbox.pb.h>
#include "TestServiceProvider.hpp"

bool isItemsConsistent(inbox::InboxService& service)
{
   common::EmptyMessage emptyMsg;
   inbox::InboxItems responce;
   
   service.GetInbox(nullptr, &emptyMsg, &responce, nullptr);
   
   return 5 == responce.items_size() &&
      "item0" == responce.items(0).text() &&
      "item1" == responce.items(1).text() &&
      "item2" == responce.items(2).text() &&
      "item3" == responce.items(3).text() &&
      "item4" == responce.items(4).text();
}

BOOST_AUTO_TEST_CASE( Inbox_GetInbox ) 
{
   TestServiceProvider<inbox::InboxService> serviceProvider;
   auto& service = serviceProvider.getService();
   
   BOOST_CHECK(isItemsConsistent(service));
}

BOOST_AUTO_TEST_CASE( Inbox_AddDeleteInbox ) 
{
   TestServiceProvider<inbox::InboxService> serviceProvider;
   auto& service = serviceProvider.getService();
   
   inbox::InboxItemInfo request;
   request.set_text("text");
   
   common::UniqueId responce;
   
   service.AddItem(nullptr, &request, &responce, nullptr);
   
   BOOST_CHECK(responce.id() != 0);
   
   {
      common::EmptyMessage emptyMsg;
      inbox::InboxItems responce;
      
      service.GetInbox(nullptr, &emptyMsg, &responce, nullptr);
      
      BOOST_CHECK_EQUAL(6, responce.items_size());
      
      auto pos = std::find_if(responce.items().begin(), responce.items().end(), 
         [](auto x){return x.text() == "text";});
      BOOST_REQUIRE(pos != responce.items().end());
      
      {
         common::UniqueId request;
         request.set_id(pos->id().id());
         
         common::OperationResultMessage opResult;
         service.DeleteItem(nullptr, &request, &opResult, nullptr);
         
         BOOST_CHECK(opResult.success());
         BOOST_CHECK(isItemsConsistent(service));
      }
   }
}

BOOST_AUTO_TEST_CASE( Inbox_DeleteWrongInbox ) 
{
   TestServiceProvider<inbox::InboxService> serviceProvider;
   auto& service = serviceProvider.getService();
   
   common::UniqueId request;
   request.set_id(50);
   
   common::OperationResultMessage opResult;
   service.DeleteItem(nullptr, &request, &opResult, nullptr);
   
   BOOST_CHECK(!opResult.success());
   BOOST_CHECK(isItemsConsistent(service));
}

BOOST_AUTO_TEST_CASE( Inbox_EditWrongInbox ) 
{
   TestServiceProvider<inbox::InboxService> serviceProvider;
   auto& service = serviceProvider.getService();
   
   inbox::InboxItemInfo request;
   request.set_text("text");
   request.mutable_id()->set_id(50);
   
   common::OperationResultMessage opResult;
   
   service.EditItem(nullptr, &request, &opResult, nullptr);
   
   BOOST_CHECK(!opResult.success());
   BOOST_CHECK(isItemsConsistent(service));
}

BOOST_AUTO_TEST_CASE( Inbox_EditInbox ) 
{
   TestServiceProvider<inbox::InboxService> serviceProvider;
   auto& service = serviceProvider.getService();
   
   inbox::InboxItemInfo request;
   request.set_text("text");
   
   common::UniqueId responce;
   
   service.AddItem(nullptr, &request, &responce, nullptr);
   
   BOOST_CHECK(responce.id() != 0);
   
   request.mutable_id()->set_id(responce.id());
   request.set_text("other_text");
   
   common::OperationResultMessage opResult;
   service.EditItem(nullptr, &request, &opResult, nullptr);
   
   BOOST_CHECK(opResult.success());
   
   {
      common::EmptyMessage emptyMsg;
      inbox::InboxItems responce;
      
      service.GetInbox(nullptr, &emptyMsg, &responce, nullptr);
      
      BOOST_CHECK_EQUAL(6, responce.items_size());
      
      auto pos = std::find_if(responce.items().begin(), responce.items().end(), 
         [](auto x){return x.text() == "other_text";});
      BOOST_REQUIRE(pos != responce.items().end());
      BOOST_CHECK(pos->id().id() == request.id().id());
      
      {
         common::UniqueId request;
         request.set_id(pos->id().id());
         
         common::OperationResultMessage opResult;
         service.DeleteItem(nullptr, &request, &opResult, nullptr);
         
         BOOST_CHECK(opResult.success());
         BOOST_CHECK(isItemsConsistent(service));
      }
   }
}
