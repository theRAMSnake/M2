#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include "boost/date_time/gregorian/gregorian.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <Client/MateriaClient.hpp>
#include <Client/ICalendar.hpp>
#include <Client/IContainer.hpp>

class CalendarTest
{
public:
   CalendarTest()
   : mClient("test")
   , mService(mClient.getCalendar())
   {
      mService.clear();

      for(int i = 0; i < 20; ++i)
      {
         boost::gregorian::date date(2018, boost::date_time::months_of_year::Feb, i + 1);
         const boost::posix_time::ptime timestamp(date);

         std::string text = "date" + boost::lexical_cast<std::string>(i);

         mSampleIds.push_back(mService.insertItem({materia::Id::Invalid, text, boost::posix_time::to_time_t(timestamp)}));
      }
   }

protected:
   std::vector<materia::CalendarItem> queryAll()
   {
      auto timefrom = boost::posix_time::ptime(
         boost::gregorian::date(2018, boost::date_time::months_of_year::Jan, 28));

      auto timeto = boost::posix_time::ptime(
         boost::gregorian::date(2018, boost::date_time::months_of_year::Feb, 26));
   
      return mService.query(boost::posix_time::to_time_t(timefrom), boost::posix_time::to_time_t(timeto));
   }

   materia::MateriaClient mClient;
   materia::ICalendar& mService;
   std::vector<materia::Id> mSampleIds;
};

BOOST_FIXTURE_TEST_CASE( Query, CalendarTest )  
{
   //query nothing
   {
      auto from = boost::posix_time::ptime(
         boost::gregorian::date(2016, boost::date_time::months_of_year::Feb, 1));

      auto to = boost::posix_time::ptime(
         boost::gregorian::date(2016, boost::date_time::months_of_year::Feb, 15));

      BOOST_CHECK(mService.query(boost::posix_time::to_time_t(from), boost::posix_time::to_time_t(to)).size() == 0);
   }
   //query from > to
   {
      auto from = boost::posix_time::ptime(
         boost::gregorian::date(2016, boost::date_time::months_of_year::Feb, 10));

      auto to = boost::posix_time::ptime(
         boost::gregorian::date(2016, boost::date_time::months_of_year::Feb, 5));

      BOOST_CHECK(mService.query(boost::posix_time::to_time_t(from), boost::posix_time::to_time_t(to)).size() == 0);
   }
   //query 5 items
   {
      auto timefrom = boost::posix_time::ptime(
         boost::gregorian::date(2018, boost::date_time::months_of_year::Jan, 30));

      auto timeto = boost::posix_time::ptime(
         boost::gregorian::date(2018, boost::date_time::months_of_year::Feb, 6));

      auto result = mService.query(boost::posix_time::to_time_t(timefrom), boost::posix_time::to_time_t(timeto));
      BOOST_CHECK_EQUAL(result.size(), 5);

      for(auto x : result)
      {
         BOOST_CHECK(boost::posix_time::to_time_t(timefrom) <= x.timestamp && x.timestamp <= boost::posix_time::to_time_t(timeto));
      }
   }
   //query all
   {  
      BOOST_CHECK(queryAll().size() == 20);
   }
}

BOOST_FIXTURE_TEST_CASE( Next, CalendarTest )  
{
   //query invalid
   {
      auto timefrom = boost::posix_time::ptime(
         boost::gregorian::date(2018, boost::date_time::months_of_year::Feb, 1));

      BOOST_CHECK(mService.next(boost::posix_time::to_time_t(timefrom), -2).empty());
   }
   //query 5
   {
      auto timefrom = boost::posix_time::ptime(
         boost::gregorian::date(2018, boost::date_time::months_of_year::Feb, 1));

      auto result = mService.next(boost::posix_time::to_time_t(timefrom), 5);
      BOOST_CHECK(result.size() == 5);

      auto timetreshold = boost::posix_time::ptime(
         boost::gregorian::date(2018, boost::date_time::months_of_year::Feb, 6));

      for(auto x : result)
      {
         BOOST_CHECK(boost::posix_time::to_time_t(timetreshold) >= x.timestamp);
      }
   }
   //query last date
   auto timefrom = boost::posix_time::ptime(
         boost::gregorian::date(2018, boost::date_time::months_of_year::Mar, 1));

   BOOST_CHECK(mService.next(boost::posix_time::to_time_t(timefrom), 5).empty());
}

BOOST_FIXTURE_TEST_CASE( CalendarTest_Delete, CalendarTest ) 
{
   //delete unexist item
   {
      BOOST_CHECK(!mService.deleteItem(materia::Id("dfdfsd")));
      //check that all items are there
      BOOST_CHECK(queryAll().size() == 20);
   }

   //delete exist item
   const int sampleItemIndex = 2;
   {
      BOOST_CHECK(mService.deleteItem(mSampleIds[sampleItemIndex]));

      //check that all, but deleted items are there
      {
         auto result = queryAll();
         BOOST_CHECK(result.size() == 19);

         bool keyFound = std::find_if(result.begin(), result.end(), [&](auto y)->bool{return y.id == mSampleIds[sampleItemIndex];})
            != result.end();

         BOOST_CHECK(!keyFound);
      }
   }

   //delete all
   for(auto x : mSampleIds)
   {
      if(mSampleIds[sampleItemIndex] == x)
      {
         //skip already deleted item
         continue;
      }

      BOOST_CHECK(mService.deleteItem(x));
   }

   //nothing can be obtained anymore
   {
      auto timefrom = boost::posix_time::ptime(
         boost::gregorian::date(2000, boost::date_time::months_of_year::Jan, 30));

      auto timeto = boost::posix_time::ptime(
         boost::gregorian::date(2030, boost::date_time::months_of_year::Feb, 6));

      BOOST_CHECK(mService.query(boost::posix_time::to_time_t(timefrom), boost::posix_time::to_time_t(timeto)).empty());
   }
}

BOOST_FIXTURE_TEST_CASE( CalendarTest_Edit, CalendarTest ) 
{
   //edit valid
   {
      auto timefrom = boost::posix_time::ptime(
         boost::gregorian::date(2019, boost::date_time::months_of_year::Mar, 1));

      BOOST_CHECK(mService.replaceItem({mSampleIds[0], "other_text", boost::posix_time::to_time_t(timefrom)}));

      {
         auto result = mService.next(boost::posix_time::to_time_t(timefrom - boost::gregorian::date_duration(
            boost::gregorian::days(1))), 1);
         BOOST_CHECK(result.size() == 1);
         BOOST_CHECK(result[0].id == mSampleIds[0]);
         BOOST_CHECK(result[0].text == "other_text");
         BOOST_CHECK(result[0].timestamp == boost::posix_time::to_time_t(timefrom));
      }
   }

   //edit wrong id
   {
      auto timefrom = boost::posix_time::ptime(
         boost::gregorian::date(2019, boost::date_time::months_of_year::Mar, 1));

      BOOST_CHECK(!mService.replaceItem({materia::Id("sdfhjksdfhjk"), "other_text", boost::posix_time::to_time_t(timefrom)}));
   }
}