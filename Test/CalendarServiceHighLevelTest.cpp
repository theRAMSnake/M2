#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <messages/calendar.pb.h>
#include <messages/database.pb.h>
#include "TestServiceProvider.hpp"
#include <boost/filesystem.hpp>
#include "boost/date_time/gregorian/gregorian.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>

#include <mongocxx/instance.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>

namespace 
{
   void cleanUp()
   {   
      mongocxx::instance instance{}; 
      mongocxx::client client{mongocxx::uri{}};

      client["materia"].drop();
   }
}

std::vector<std::string> fillSampleItems(calendar::CalendarService& service)
{
   cleanUp();
   
   std::vector<std::string> result;

   for(int i = 0; i < 20; ++i)
   {
      boost::gregorian::date date(2018, boost::date_time::months_of_year::Feb, i + 1);
      const boost::posix_time::ptime timestamp(date);

      std::string text = "date" + boost::lexical_cast<std::string>(i);

      calendar::CalendarItem item;
      item.set_timestamp(boost::posix_time:: to_time_t(timestamp));
      item.set_text(text);

      common::UniqueId id;
      service.AddItem(nullptr, &item, &id, nullptr);

      result.push_back(id.guid());
   }

   return result;
}

BOOST_AUTO_TEST_CASE( Calendar_Query )
{
   TestServiceProvider<calendar::CalendarService> serviceProvider;
   auto& service = serviceProvider.getService();

   std::vector<std::string> guids = fillSampleItems(service);

   /*{
      TestServiceProvider<database::DatabaseService> serviceProvider;
      auto& service = serviceProvider.getService();

      common::EmptyMessage emptyMsg;
      
      database::Documents fetchResult;
      service.Fetch(nullptr, &emptyMsg, &fetchResult, nullptr);

      BOOST_CHECK_EQUAL(fetchResult.result_size(), 11);

      for(auto x : fetchResult.result())
      {
         printf("F:%s\n", x.DebugString().c_str());
      }
   }*/

   //query nothing
   {
      calendar::TimeRange timeRange;
      timeRange.set_timestampfrom(boost::posix_time:: to_time_t(boost::posix_time::ptime(
         boost::gregorian::date(2016, boost::date_time::months_of_year::Feb, 1))));

      timeRange.set_timestampto(boost::posix_time:: to_time_t(boost::posix_time::ptime(
         boost::gregorian::date(2016, boost::date_time::months_of_year::Feb, 15))));

      calendar::CalendarItems result;

      service.Query(nullptr, &timeRange, &result, nullptr);
      BOOST_CHECK(result.items_size() == 0);
   }
   //query from > to
   {
      calendar::TimeRange timeRange;
      timeRange.set_timestampfrom(boost::posix_time:: to_time_t(boost::posix_time::ptime(
         boost::gregorian::date(2018, boost::date_time::months_of_year::Feb, 10))));

      timeRange.set_timestampto(boost::posix_time:: to_time_t(boost::posix_time::ptime(
         boost::gregorian::date(2018, boost::date_time::months_of_year::Feb, 5))));

      calendar::CalendarItems result;

      service.Query(nullptr, &timeRange, &result, nullptr);
      BOOST_CHECK(result.items_size() == 0);
   }
   //query 5 items
   {
      int timefrom = boost::posix_time:: to_time_t(boost::posix_time::ptime(
         boost::gregorian::date(2018, boost::date_time::months_of_year::Jan, 30)));

      int timeto = boost::posix_time:: to_time_t(boost::posix_time::ptime(
         boost::gregorian::date(2018, boost::date_time::months_of_year::Feb, 6)));

      printf("query %d-%d", timefrom, timeto);

      calendar::TimeRange timeRange;

      timeRange.set_timestampfrom(timefrom);
      timeRange.set_timestampto(timeto);

      calendar::CalendarItems result;

      service.Query(nullptr, &timeRange, &result, nullptr);
      BOOST_CHECK_EQUAL(result.items_size(), 5);

      for(auto x : result.items())
      {
         BOOST_CHECK(timefrom <= x.timestamp() && x.timestamp() <= timeto);
      }
   }
   //query all
   {
      calendar::TimeRange timeRange;
      timeRange.set_timestampfrom(boost::posix_time:: to_time_t(boost::posix_time::ptime(
         boost::gregorian::date(2018, boost::date_time::months_of_year::Jan, 28))));
   
      timeRange.set_timestampto(boost::posix_time:: to_time_t(boost::posix_time::ptime(
         boost::gregorian::date(2018, boost::date_time::months_of_year::Feb, 26))));
   
      calendar::CalendarItems result;
   
      service.Query(nullptr, &timeRange, &result, nullptr);
      BOOST_CHECK(result.items_size() == 20);
   }
}

BOOST_AUTO_TEST_CASE( Calendar_Next )
{
   TestServiceProvider<calendar::CalendarService> serviceProvider;
   auto& service = serviceProvider.getService();

   std::vector<std::string> guids = fillSampleItems(service);

   //query invalid
   {
      calendar::NextQueryParameters query;
      int timefrom = boost::posix_time:: to_time_t(boost::posix_time::ptime(
         boost::gregorian::date(2018, boost::date_time::months_of_year::Feb, 1)));
      query.set_timestampfrom(timefrom);
      query.set_limit(-2);

      calendar::CalendarItems result;
      
      service.Next(nullptr, &query, &result, nullptr);
      BOOST_CHECK(result.items_size() == 0);
   }
   //query 5
   {
      calendar::NextQueryParameters query;
      int timefrom = boost::posix_time:: to_time_t(boost::posix_time::ptime(
         boost::gregorian::date(2018, boost::date_time::months_of_year::Feb, 1)));
      query.set_timestampfrom(timefrom);
      query.set_limit(5);

      calendar::CalendarItems result;
      
      service.Next(nullptr, &query, &result, nullptr);
      BOOST_CHECK(result.items_size() == 5);

      int timetreshold = boost::posix_time:: to_time_t(boost::posix_time::ptime(
         boost::gregorian::date(2018, boost::date_time::months_of_year::Feb, 6)));

      for(auto x : result.items())
      {
         BOOST_CHECK(timetreshold >= x.timestamp());
      }
   }
   //query last date
   calendar::NextQueryParameters query;
   int timefrom = boost::posix_time:: to_time_t(boost::posix_time::ptime(
      boost::gregorian::date(2018, boost::date_time::months_of_year::Mar, 1)));
   query.set_timestampfrom(timefrom);
   query.set_limit(5);

   calendar::CalendarItems result;
   
   service.Next(nullptr, &query, &result, nullptr);
   BOOST_CHECK(result.items_size() == 0);
}

BOOST_AUTO_TEST_CASE( Calendar_Delete )
{
   TestServiceProvider<calendar::CalendarService> serviceProvider;
   auto& service = serviceProvider.getService();

   std::vector<std::string> guids = fillSampleItems(service);

   //delete unexist item
   {
      common::UniqueId id;
      id.set_guid("sdgs");
      
      common::OperationResultMessage result;

      service.DeleteItem(nullptr, &id, &result, nullptr);
      BOOST_CHECK(!result.success());

      //check that all items are there
      for(auto x : guids)
      {
         calendar::TimeRange timeRange;
         timeRange.set_timestampfrom(boost::posix_time:: to_time_t(boost::posix_time::ptime(
            boost::gregorian::date(2018, boost::date_time::months_of_year::Jan, 28))));
      
         timeRange.set_timestampto(boost::posix_time:: to_time_t(boost::posix_time::ptime(
            boost::gregorian::date(2018, boost::date_time::months_of_year::Feb, 28))));
      
         calendar::CalendarItems result;
      
         service.Query(nullptr, &timeRange, &result, nullptr);
         BOOST_CHECK(result.items_size() == 20);
      }
   }

   //delete exist item
   const int sampleItemIndex = 2;
   {
      
      common::UniqueId id;
      id.set_guid(guids[sampleItemIndex]);
      
      common::OperationResultMessage result;

      service.DeleteItem(nullptr, &id, &result, nullptr);
      BOOST_CHECK(result.success());

      //check that all, but deleted items are there
      {
         calendar::TimeRange timeRange;
         timeRange.set_timestampfrom(boost::posix_time:: to_time_t(boost::posix_time::ptime(
            boost::gregorian::date(2018, boost::date_time::months_of_year::Jan, 28))));
      
         timeRange.set_timestampto(boost::posix_time:: to_time_t(boost::posix_time::ptime(
            boost::gregorian::date(2018, boost::date_time::months_of_year::Feb, 28))));
      
         calendar::CalendarItems result;
      
         service.Query(nullptr, &timeRange, &result, nullptr);
         BOOST_CHECK(result.items_size() == 19);

         bool keyFound = std::find_if(result.items().begin(), result.items().end(), [&](auto y)->bool{return y.id().guid() == guids[sampleItemIndex];})
            != result.items().end();

         BOOST_CHECK(!keyFound);
      }
   }

   //delete all
   for(auto x : guids)
   {
      if(guids[sampleItemIndex] == x)
      {
         //skip already deleted item
         continue;
      }

      common::UniqueId id;
      id.set_guid(x);
      
      common::OperationResultMessage result;

      service.DeleteItem(nullptr, &id, &result, nullptr);
      BOOST_CHECK(result.success());
   }

   //nothing can be obtained anymore
   {
      calendar::TimeRange timeRange;
      timeRange.set_timestampfrom(boost::posix_time:: to_time_t(boost::posix_time::ptime(
         boost::gregorian::date(2000, boost::date_time::months_of_year::Jan, 28))));
   
      timeRange.set_timestampto(boost::posix_time:: to_time_t(boost::posix_time::ptime(
         boost::gregorian::date(2020, boost::date_time::months_of_year::Feb, 28))));
   
      calendar::CalendarItems result;
   
      service.Query(nullptr, &timeRange, &result, nullptr);
      BOOST_CHECK(result.items_size() == 0);
   }
}

BOOST_AUTO_TEST_CASE( Calendar_Edit )
{
   TestServiceProvider<calendar::CalendarService> serviceProvider;
   auto& service = serviceProvider.getService();

   std::vector<std::string> guids = fillSampleItems(service);

   //edit valid
   {
      calendar::CalendarItem item;
      item.mutable_id()->set_guid(guids[0]);
      item.set_text("other_text");
      
      int timefrom = boost::posix_time:: to_time_t(boost::posix_time::ptime(
         boost::gregorian::date(2019, boost::date_time::months_of_year::Mar, 1)));

      item.set_timestamp(timefrom);

      common::OperationResultMessage result;
      
      service.EditItem(nullptr, &item, &result, nullptr);
      BOOST_CHECK(result.success());

      {
         calendar::NextQueryParameters query;
         query.set_timestampfrom(timefrom - 1);
         query.set_limit(1);
   
         calendar::CalendarItems result;
         
         service.Next(nullptr, &query, &result, nullptr);
         BOOST_CHECK(result.items_size() == 1);
         BOOST_CHECK(result.items(0).id().guid() == guids[0]);
         BOOST_CHECK(result.items(0).text() == "other_text");
         BOOST_CHECK(result.items(0).timestamp() == timefrom);
      }
   }

   //edit wrong id
   {
      calendar::CalendarItem item;
      item.mutable_id()->set_guid("sdfhjksdfhjk");
      item.set_text("other_text");

      int timefrom = boost::posix_time:: to_time_t(boost::posix_time::ptime(
         boost::gregorian::date(2019, boost::date_time::months_of_year::Mar, 1)));

      item.set_timestamp(timefrom);

      common::OperationResultMessage result;
      
      service.EditItem(nullptr, &item, &result, nullptr);
      BOOST_CHECK(!result.success());
   }
}