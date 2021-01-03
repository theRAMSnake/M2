#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <Core/ICore3.hpp>
#include "../Core/private/JsonSerializer.hpp"
#include "Utils.hpp"

class RewardTest
{
public:
   RewardTest()
   {
      system("rm Test.db");
      mCore = materia::createCore({"Test.db"});
   }

protected:

   std::shared_ptr<materia::ICore3> mCore;
};

BOOST_FIXTURE_TEST_CASE( AddPoints, RewardTest ) 
{
   boost::property_tree::ptree create;
   create.put("operation", "create");
   create.put("typename", "reward_pool");
   create.put("defined_id", "p1");
   create.put("params.amount", 0);
   create.put("params.amountMax", 5);

   mCore->executeCommandJson(writeJson(create));

   create.put("defined_id", "p2");
   create.put("params.amount", 3);
   create.put("params.amountMax", 5);

   mCore->executeCommandJson(writeJson(create));

   create.put("defined_id", "p3");
   create.put("params.amount", 2);
   create.put("params.amountMax", 5);

   mCore->executeCommandJson(writeJson(create));

   boost::property_tree::ptree rwd;
   rwd.put("operation", "reward");
   rwd.put("points", 11);

   mCore->executeCommandJson(writeJson(rwd));

   boost::property_tree::ptree query;
   query.put("operation", "query");
   query.put("filter", "IS(reward_pool)");
   auto result = mCore->executeCommandJson(writeJson(query));

   {
      auto ol = readJson<boost::property_tree::ptree>(result);
      
      for(auto& v : ol.get_child("object_list"))
      {
         auto a = v.second.get<int>("amount");
         BOOST_CHECK_EQUAL(5, a);
      }
   }

   rwd.put("points", -100);

   mCore->executeCommandJson(writeJson(rwd));
   result = mCore->executeCommandJson(writeJson(query));

   {
      auto ol = readJson<boost::property_tree::ptree>(result);
      
      for(auto& v : ol.get_child("object_list"))
      {
         auto a = v.second.get<int>("amount");
         BOOST_CHECK_EQUAL(0, a);
      }
   }
}

BOOST_FIXTURE_TEST_CASE( AddWorkburden, RewardTest ) 
{
   boost::property_tree::ptree create;
   create.put("operation", "create");
   create.put("typename", "object");
   create.put("defined_id", "config.reward");
   create.put("params.workburdenPerDay", 200);

   mCore->executeCommandJson(writeJson(create));

   mCore->onNewDay(boost::gregorian::date(2021, boost::gregorian::Jan, 1)); //Friday
   auto v = *query("work.burden", *mCore);
   BOOST_CHECK_EQUAL("200", v.get<std::string>("value"));

   mCore->onNewDay(boost::gregorian::date(2021, boost::gregorian::Jan, 2));
   v = *query("work.burden", *mCore);
   BOOST_CHECK_EQUAL("200", v.get<std::string>("value"));

   mCore->onNewDay(boost::gregorian::date(2021, boost::gregorian::Jan, 3));
   v = *query("work.burden", *mCore);
   BOOST_CHECK_EQUAL("200", v.get<std::string>("value"));

   mCore->onNewDay(boost::gregorian::date(2021, boost::gregorian::Jan, 4));
   v = *query("work.burden", *mCore);
   BOOST_CHECK_EQUAL("400", v.get<std::string>("value"));

   mCore->onNewDay(boost::gregorian::date(2021, boost::gregorian::Jan, 5));
   v = *query("work.burden", *mCore);
   BOOST_CHECK_EQUAL("600", v.get<std::string>("value"));
}

BOOST_FIXTURE_TEST_CASE( AddPointsModifiedPosPosTest, RewardTest ) 
{
   {
      boost::property_tree::ptree create;
      create.put("operation", "create");
      create.put("typename", "reward_pool");
      create.put("defined_id", "p1");
      create.put("params.amount", 0);
      create.put("params.amountMax", 100);

      mCore->executeCommandJson(writeJson(create));
   }
   {
      boost::property_tree::ptree create;
      create.put("operation", "create");
      create.put("typename", "reward_modifier");
      create.put("params.value", 0.1);

      mCore->executeCommandJson(writeJson(create));

      create.put("params.value", 0.25);

      mCore->executeCommandJson(writeJson(create));

      create.put("params.value", -0.1);

      mCore->executeCommandJson(writeJson(create));
   }

   boost::property_tree::ptree rwd;
   rwd.put("operation", "reward");
   rwd.put("points", 10);
   mCore->executeCommandJson(writeJson(rwd));

   auto v = *query("p1", *mCore);
   BOOST_CHECK_EQUAL("12", v.get<std::string>("amount"));

   mCore->executeCommandJson(writeJson(rwd));

   v = *query("p1", *mCore);
   BOOST_CHECK_EQUAL("25", v.get<std::string>("amount"));
}

BOOST_FIXTURE_TEST_CASE( AddPointsModifiedPosNegTest, RewardTest ) 
{
   {
      boost::property_tree::ptree create;
      create.put("operation", "create");
      create.put("typename", "reward_pool");
      create.put("defined_id", "p1");
      create.put("params.amount", 0);
      create.put("params.amountMax", 100);

      mCore->executeCommandJson(writeJson(create));
   }
   {
      boost::property_tree::ptree create;
      create.put("operation", "create");
      create.put("typename", "reward_modifier");
      create.put("params.value", 0.1);

      mCore->executeCommandJson(writeJson(create));

      create.put("params.value", -0.25);

      mCore->executeCommandJson(writeJson(create));

      create.put("params.value", -0.1);

      mCore->executeCommandJson(writeJson(create));
   }

   boost::property_tree::ptree rwd;
   rwd.put("operation", "reward");
   rwd.put("points", 10);
   mCore->executeCommandJson(writeJson(rwd));

   auto v = *query("p1", *mCore);
   BOOST_CHECK_EQUAL("7", v.get<std::string>("amount"));

   mCore->executeCommandJson(writeJson(rwd));

   v = *query("p1", *mCore);
   BOOST_CHECK_EQUAL("15", v.get<std::string>("amount"));
}

BOOST_FIXTURE_TEST_CASE( AddPointsModifiedNegTest, RewardTest ) 
{
   {
      boost::property_tree::ptree create;
      create.put("operation", "create");
      create.put("typename", "reward_pool");
      create.put("defined_id", "p1");
      create.put("params.amount", 50);
      create.put("params.amountMax", 100);

      mCore->executeCommandJson(writeJson(create));
   }
   {
      boost::property_tree::ptree create;
      create.put("operation", "create");
      create.put("typename", "reward_modifier");
      create.put("params.value", 0.1);

      mCore->executeCommandJson(writeJson(create));

      create.put("params.value", 0.25);

      mCore->executeCommandJson(writeJson(create));

      create.put("params.value", -0.1);

      mCore->executeCommandJson(writeJson(create));
   }

   boost::property_tree::ptree rwd;
   rwd.put("operation", "reward");
   rwd.put("points", -10);
   mCore->executeCommandJson(writeJson(rwd));

   auto v = *query("p1", *mCore);
   BOOST_CHECK_EQUAL("43", v.get<std::string>("amount"));

   mCore->executeCommandJson(writeJson(rwd));

   v = *query("p1", *mCore);
   BOOST_CHECK_EQUAL("35", v.get<std::string>("amount"));
}

BOOST_FIXTURE_TEST_CASE( PresetModifiersTest, RewardTest ) 
{
   //Inbox, calendar, workburden - all active
   set("work.burden", 0, *mCore);

   mCore->onNewDay(boost::gregorian::date(2021, boost::gregorian::Jan, 2));

   BOOST_CHECK(query("mod.inbox", *mCore));
   BOOST_CHECK(query("mod.calendar", *mCore));
   BOOST_CHECK(query("mod.workburden", *mCore));
   BOOST_CHECK_EQUAL(0.1, query("mod.workburden", *mCore)->get<double>("value"));

   //Inbox, calendar, workburden - all disabled
   boost::property_tree::ptree push;
   push.put("operation", "push");
   push.put("listId", "inbox");
   push.put("value", "val");
   mCore->executeCommandJson(writeJson(push));

   boost::property_tree::ptree create;
   create.put("operation", "create");
   create.put("typename", "calendar_item");
   create.put("params.timestamp", 25);
   create.put("params.entityType", 1);
   mCore->executeCommandJson(writeJson(create));

   set("work.burden", 500, *mCore);

   mCore->onNewDay(boost::gregorian::date(2021, boost::gregorian::Jan, 2));

   BOOST_CHECK(!query("mod.inbox", *mCore));
   BOOST_CHECK(!query("mod.calendar", *mCore));
   BOOST_CHECK(query("mod.workburden", *mCore));
   BOOST_CHECK_EQUAL(-0.2, query("mod.workburden", *mCore)->get<double>("value"));
}