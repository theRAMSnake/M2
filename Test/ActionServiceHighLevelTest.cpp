#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <messages/actions.pb.h>
#include <boost/filesystem.hpp>
#include "TestServiceProvider.hpp"

#include <mongocxx/instance.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>

namespace 
{
   void cleanUp()
   {   
      boost::filesystem::remove_all("actions_service_data");
      boost::filesystem::create_directory("actions_service_data");

      mongocxx::instance instance{}; 
      mongocxx::client client{mongocxx::uri{}};

      client["materia"].drop();
   }
}

BOOST_AUTO_TEST_CASE( Actions_AddDeleteAction_Parentless ) 
{
   cleanUp();
   TestServiceProvider<actions::ActionsService> serviceProvider;
   auto& service = serviceProvider.getService();

   common::UniqueId id1;
   common::UniqueId id2;
   
   {
      actions::ActionInfo request;
      request.set_title("text");
      request.set_description("description");
      request.set_type(actions::ActionType::Task);
      
      service.AddElement(nullptr, &request, &id1, nullptr);
   }
   {
      actions::ActionInfo request;
      request.set_title("text2");
      request.set_description("description2");
      request.set_type(actions::ActionType::Group);
      
      service.AddElement(nullptr, &request, &id2, nullptr);
   }
   BOOST_CHECK(id1.guid() != id2.guid());

   {
      common::EmptyMessage emptyMsg;
      actions::ActionsList responce;
      
      service.GetParentlessElements(nullptr, &emptyMsg, &responce, nullptr);
      
      BOOST_CHECK_EQUAL(2, responce.list_size());
      
      {
         auto pos = std::find_if(responce.list().begin(), responce.list().end(), 
            [&](auto x){return x.id().guid() == id1.guid();});
         BOOST_REQUIRE(pos != responce.list().end());
      }
      {
         auto pos = std::find_if(responce.list().begin(), responce.list().end(), 
            [&](auto x){return x.id().guid() == id2.guid();});
         BOOST_REQUIRE(pos != responce.list().end());
      }
      
      {
         common::UniqueId request;
         request.set_guid(id1.guid());
         
         common::OperationResultMessage opResult;
         service.DeleteElement(nullptr, &request, &opResult, nullptr);
         
         BOOST_CHECK(opResult.success());
      }
      {
         common::EmptyMessage emptyMsg;
         actions::ActionsList responce;
         
         service.GetParentlessElements(nullptr, &emptyMsg, &responce, nullptr);
         
         BOOST_CHECK_EQUAL(1, responce.list_size());

         auto pos = std::find_if(responce.list().begin(), responce.list().end(), 
            [&](auto x){return x.id().guid() == id2.guid();});
         BOOST_REQUIRE(pos != responce.list().end());
      }
      {
         common::UniqueId request;
         request.set_guid(id2.guid());
         
         common::OperationResultMessage opResult;
         service.DeleteElement(nullptr, &request, &opResult, nullptr);
         
         BOOST_CHECK(opResult.success());
      }
      {
         common::EmptyMessage emptyMsg;
         actions::ActionsList responce;
         
         service.GetParentlessElements(nullptr, &emptyMsg, &responce, nullptr);
         
         BOOST_CHECK_EQUAL(0, responce.list_size());
      }
   }
}

BOOST_AUTO_TEST_CASE( Actions_AddDeleteAction_Parented ) 
{
   cleanUp();

   TestServiceProvider<actions::ActionsService> serviceProvider;
   auto& service = serviceProvider.getService();

   common::UniqueId parentId;
   common::UniqueId childId1;
   common::UniqueId childId2;
   common::UniqueId nonChildId;
   
   {
      actions::ActionInfo request;
      request.set_title("text");
      request.set_description("description");
      request.set_type(actions::ActionType::Group);
      
      service.AddElement(nullptr, &request, &parentId, nullptr);
   }
   {
      actions::ActionInfo request;
      request.set_title("text2");
      request.set_description("description2");
      request.set_type(actions::ActionType::Task);
      request.mutable_parentid()->set_guid(parentId.guid());
      
      service.AddElement(nullptr, &request, &childId1, nullptr);
      service.AddElement(nullptr, &request, &childId2, nullptr);
   }
   {
      actions::ActionInfo request;
      request.set_title("text3");
      request.set_description("description3");
      request.set_type(actions::ActionType::Task);
      
      service.AddElement(nullptr, &request, &nonChildId, nullptr);
   }

   {
      common::EmptyMessage emptyMsg;
      actions::ActionsList responce;
      
      service.GetParentlessElements(nullptr, &emptyMsg, &responce, nullptr);
      
      BOOST_CHECK_EQUAL(2, responce.list_size());
      
      {
         auto pos = std::find_if(responce.list().begin(), responce.list().end(), 
            [&](auto x){return x.id().guid() == parentId.guid();});
         BOOST_REQUIRE(pos != responce.list().end());
      }
      {
         auto pos = std::find_if(responce.list().begin(), responce.list().end(), 
            [&](auto x){return x.id().guid() == nonChildId.guid();});
         BOOST_REQUIRE(pos != responce.list().end());
      }
   }

   {
      actions::ActionsList responce;
      
      service.GetChildren(nullptr, &parentId, &responce, nullptr);
      
      BOOST_CHECK_EQUAL(2, responce.list_size());
      
      {
         auto pos = std::find_if(responce.list().begin(), responce.list().end(), 
            [&](auto x){return x.id().guid() == childId1.guid();});
         BOOST_REQUIRE(pos != responce.list().end());
      }
      {
         auto pos = std::find_if(responce.list().begin(), responce.list().end(), 
            [&](auto x){return x.id().guid() == childId2.guid();});
         BOOST_REQUIRE(pos != responce.list().end());
      }
   }
      
   {
      common::OperationResultMessage opResult;
      service.DeleteElement(nullptr, &parentId, &opResult, nullptr);
      
      BOOST_CHECK(opResult.success());
   }

   {
      common::EmptyMessage emptyMsg;
      actions::ActionsList responce;
      
      service.GetParentlessElements(nullptr, &emptyMsg, &responce, nullptr);
      
      BOOST_CHECK_EQUAL(1, responce.list_size());
      
      {
         auto pos = std::find_if(responce.list().begin(), responce.list().end(), 
            [&](auto x){return x.id().guid() == nonChildId.guid();});
         BOOST_REQUIRE(pos != responce.list().end());
      }
   }

   {
      actions::ActionsList responce;
      service.GetChildren(nullptr, &parentId, &responce, nullptr);
      BOOST_CHECK_EQUAL(0, responce.list_size());
   }

   {
      common::OperationResultMessage opResult;
      service.DeleteElement(nullptr, &nonChildId, &opResult, nullptr);
      
      BOOST_CHECK(opResult.success());
   }

   {
      common::EmptyMessage emptyMsg;
      actions::ActionsList responce;
      
      service.GetParentlessElements(nullptr, &emptyMsg, &responce, nullptr);
      
      BOOST_CHECK_EQUAL(0, responce.list_size());
   }
}

BOOST_AUTO_TEST_CASE( Actions_DeleteWrongAction ) 
{
   cleanUp();
   TestServiceProvider<actions::ActionsService> serviceProvider;
   auto& service = serviceProvider.getService();
   
   common::UniqueId request;
   request.set_guid("50");
   
   common::OperationResultMessage opResult;
   service.DeleteElement(nullptr, &request, &opResult, nullptr);
   
   BOOST_CHECK(!opResult.success());
}

BOOST_AUTO_TEST_CASE( Actions_EditWrongAction ) 
{
   cleanUp();
   TestServiceProvider<actions::ActionsService> serviceProvider;
   auto& service = serviceProvider.getService();
   
   actions::ActionInfo request;
   request.set_title("text2");
   request.set_description("description2");
   request.set_type(actions::ActionType::Task);
   request.mutable_parentid()->set_guid("fhdjskghdf");
   
   common::OperationResultMessage opResult;
   service.EditElement(nullptr, &request, &opResult, nullptr);
   
   BOOST_CHECK(!opResult.success());
}

BOOST_AUTO_TEST_CASE( Actions_EditAction_NoReparent ) 
{
   cleanUp();
   TestServiceProvider<actions::ActionsService> serviceProvider;
   auto& service = serviceProvider.getService();
   
   common::UniqueId id;

   actions::ActionInfo request;
   request.set_title("text");
   request.set_description("description");
   request.set_type(actions::ActionType::Group);
   
   service.AddElement(nullptr, &request, &id, nullptr);
   
   common::OperationResultMessage opResult;
   request.set_title("other_title");
   request.set_description("other_description");
   request.set_type(actions::ActionType::Task);
   request.mutable_id()->set_guid(id.guid());

   service.EditElement(nullptr, &request, &opResult, nullptr);
   
   BOOST_CHECK(opResult.success());

   {
      common::EmptyMessage emptyMsg;
      actions::ActionsList responce;
      
      service.GetParentlessElements(nullptr, &emptyMsg, &responce, nullptr);
      
      BOOST_CHECK_EQUAL(1, responce.list_size());
      BOOST_CHECK(responce.list(0).title() == "other_title");
      BOOST_CHECK(responce.list(0).description() == "other_description");
      BOOST_CHECK(responce.list(0).type() == actions::ActionType::Task);
      BOOST_CHECK(responce.list(0).id().guid() == id.guid());
   }
}

BOOST_AUTO_TEST_CASE( Actions_EditAction_Reparent ) 
{
   cleanUp();
   TestServiceProvider<actions::ActionsService> serviceProvider;
   auto& service = serviceProvider.getService();
   
   common::UniqueId parentId1;
   common::UniqueId parentId2;
   common::UniqueId childId;
   
   {
      actions::ActionInfo request;
      request.set_title("text");
      request.set_description("description");
      request.set_type(actions::ActionType::Group);
      
      service.AddElement(nullptr, &request, &parentId1, nullptr);
      service.AddElement(nullptr, &request, &parentId2, nullptr);
   }
   {
      actions::ActionInfo request;
      request.set_title("text2");
      request.set_description("description2");
      request.set_type(actions::ActionType::Task);
      request.mutable_parentid()->set_guid(parentId1.guid());
      
      service.AddElement(nullptr, &request, &childId, nullptr);
   }
   
   actions::ActionInfo request;
   common::OperationResultMessage opResult;
   request.set_title("other_title");
   request.set_description("other_description");
   request.set_type(actions::ActionType::Task);
   request.mutable_id()->set_guid(childId.guid());
   request.mutable_parentid()->set_guid(parentId2.guid());

   service.EditElement(nullptr, &request, &opResult, nullptr);
   
   BOOST_CHECK(opResult.success());

   {
      actions::ActionsList responce;
      service.GetChildren(nullptr, &parentId1, &responce, nullptr);
      BOOST_CHECK_EQUAL(0, responce.list_size());
   }
   {
      actions::ActionsList responce;
      service.GetChildren(nullptr, &parentId2, &responce, nullptr);
      BOOST_CHECK_EQUAL(1, responce.list_size());

      BOOST_CHECK(responce.list(0).title() == "other_title");
      BOOST_CHECK(responce.list(0).description() == "other_description");
      BOOST_CHECK(responce.list(0).type() == actions::ActionType::Task);
      BOOST_CHECK(responce.list(0).id().guid() == childId.guid());
   }
}

BOOST_AUTO_TEST_CASE( Actions_EditAction_Deparent ) 
{
   cleanUp();
   TestServiceProvider<actions::ActionsService> serviceProvider;
   auto& service = serviceProvider.getService();
   
   common::UniqueId parentId;
   common::UniqueId childId;
   
   {
      actions::ActionInfo request;
      request.set_title("text");
      request.set_description("description");
      request.set_type(actions::ActionType::Group);
      
      service.AddElement(nullptr, &request, &parentId, nullptr);
   }
   {
      actions::ActionInfo request;
      request.set_title("text2");
      request.set_description("description2");
      request.set_type(actions::ActionType::Task);
      request.mutable_parentid()->set_guid(parentId.guid());
      
      service.AddElement(nullptr, &request, &childId, nullptr);
   }
   
   actions::ActionInfo request;
   common::OperationResultMessage opResult;
   request.set_title("other_title");
   request.set_description("other_description");
   request.set_type(actions::ActionType::Task);
   request.mutable_id()->set_guid(childId.guid());

   service.EditElement(nullptr, &request, &opResult, nullptr);
   
   BOOST_CHECK(opResult.success());

   {
      actions::ActionsList responce;
      service.GetChildren(nullptr, &parentId, &responce, nullptr);
      BOOST_CHECK_EQUAL(0, responce.list_size());
   }
   {
      common::EmptyMessage dummy;
      actions::ActionsList responce;
      service.GetParentlessElements(nullptr, &dummy, &responce, nullptr);
      BOOST_CHECK_EQUAL(2, responce.list_size());

      for(int i = 0; i < responce.list_size(); ++i)
      {
         if(responce.list(i).id().guid() == childId.guid())
         {
            BOOST_CHECK(responce.list(i).title() == "other_title");
            BOOST_CHECK(responce.list(i).description() == "other_description");
            BOOST_CHECK(responce.list(i).type() == actions::ActionType::Task);
         }
      }
   }
}

BOOST_AUTO_TEST_CASE( Actions_EditAction_WrongParent ) 
{
   cleanUp();
   TestServiceProvider<actions::ActionsService> serviceProvider;
   auto& service = serviceProvider.getService();
   
   common::UniqueId id;

   actions::ActionInfo request;
   request.set_title("text");
   request.set_description("description");
   request.set_type(actions::ActionType::Group);
   
   service.AddElement(nullptr, &request, &id, nullptr);
   
   {
      common::OperationResultMessage opResult;
      request.set_title("other_title");
      request.set_description("other_description");
      request.set_type(actions::ActionType::Task);
      request.mutable_id()->set_guid(id.guid());
      request.mutable_parentid()->set_guid("random shit");

      service.EditElement(nullptr, &request, &opResult, nullptr);
      
      BOOST_CHECK(!opResult.success());
      {
         common::EmptyMessage dummy;
         actions::ActionsList responce;
         service.GetParentlessElements(nullptr, &dummy, &responce, nullptr);
         BOOST_CHECK_EQUAL(1, responce.list_size());

         BOOST_CHECK(responce.list(0).title() == "text");
         BOOST_CHECK(responce.list(0).description() == "description");
         BOOST_CHECK(responce.list(0).type() == actions::ActionType::Group);
         BOOST_CHECK(responce.list(0).id().guid() == id.guid());
      }
   }
   {
      common::OperationResultMessage opResult;
      request.set_title("other_title");
      request.set_description("other_description");
      request.set_type(actions::ActionType::Task);
      request.mutable_id()->set_guid(id.guid());
      request.mutable_parentid()->set_guid(id.guid());

      service.EditElement(nullptr, &request, &opResult, nullptr);
      
      BOOST_CHECK(!opResult.success());
      {
         common::EmptyMessage dummy;
         actions::ActionsList responce;
         service.GetParentlessElements(nullptr, &dummy, &responce, nullptr);
         BOOST_CHECK_EQUAL(1, responce.list_size());

         BOOST_CHECK(responce.list(0).title() == "text");
         BOOST_CHECK(responce.list(0).description() == "description");
         BOOST_CHECK(responce.list(0).type() == actions::ActionType::Group);
         BOOST_CHECK(responce.list(0).id().guid() == id.guid());
      }
   }
}

BOOST_AUTO_TEST_CASE( Actions_AddAction_WrongParent ) 
{
   cleanUp();
   TestServiceProvider<actions::ActionsService> serviceProvider;
   auto& service = serviceProvider.getService();
   
   common::UniqueId id;

   actions::ActionInfo request;
   request.set_title("text");
   request.set_description("description");
   request.set_type(actions::ActionType::Group);
   request.mutable_parentid()->set_guid("random shit");
   
   service.AddElement(nullptr, &request, &id, nullptr);
   BOOST_CHECK(id.guid().empty());
}