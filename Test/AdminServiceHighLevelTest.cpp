#define BOOST_TEST_MODULE Admin
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <messages/admin.pb.h>
#include "TestServiceProvider.hpp"

BOOST_AUTO_TEST_CASE( Admin_GetComponentInfo ) 
{
   TestServiceProvider<admin::AdminService> serviceProvider;
   auto& service = serviceProvider.getService();
   
   common::EmptyMessage emptyMsg;
   
   admin::ComponentInfoResponce responce;
   service.GetComponentInfo(nullptr, &emptyMsg, &responce, nullptr);
   
   BOOST_CHECK_EQUAL(5, responce.componentinfos_size());
   BOOST_CHECK_EQUAL(true, responce.componentinfos(0).status());
   BOOST_CHECK_EQUAL("AdminService", responce.componentinfos(0).name());
   
   BOOST_CHECK_EQUAL(true, responce.componentinfos(1).status());
   BOOST_CHECK_EQUAL("ActionsService", responce.componentinfos(1).name());

   BOOST_CHECK_EQUAL(true, responce.componentinfos(2).status());
   BOOST_CHECK_EQUAL("CalendarService", responce.componentinfos(2).name());

   BOOST_CHECK_EQUAL(true, responce.componentinfos(3).status());
   BOOST_CHECK_EQUAL("DatabaseService", responce.componentinfos(3).name());
   
   BOOST_CHECK_EQUAL(true, responce.componentinfos(4).status());
   BOOST_CHECK_EQUAL("InboxService", responce.componentinfos(4).name());
}
