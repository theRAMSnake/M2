#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <Core/ICore3.hpp>
#include "../Core/private/JsonSerializer.hpp"
#include "Utils.hpp"

#include <boost/date_time/gregorian/greg_date.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

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

   BOOST_CHECK_EQUAL(1250, queryVar("reward.points", *mCore));

   mCore->executeCommandJson(writeJson(rwd));

   BOOST_CHECK_EQUAL(2500, queryVar("reward.points", *mCore));
}

BOOST_FIXTURE_TEST_CASE( AddPointsModifiedPosNegTest, RewardTest ) 
{
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

   BOOST_CHECK_EQUAL(750, queryVar("reward.points", *mCore));

   mCore->executeCommandJson(writeJson(rwd));

   BOOST_CHECK_EQUAL(1500, queryVar("reward.points", *mCore));
}

BOOST_FIXTURE_TEST_CASE( AddPointsModifiedNegTest, RewardTest ) 
{
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

   BOOST_CHECK_EQUAL(-750, queryVar("reward.points", *mCore));

   mCore->executeCommandJson(writeJson(rwd));

   BOOST_CHECK_EQUAL(-1500, queryVar("reward.points", *mCore));
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
   create.put("params.entityTypeChoice", "Task");
   mCore->executeCommandJson(writeJson(create));

   set("work.burden", 500, *mCore);
   set("reward.points", -5, *mCore);

   mCore->onNewDay(boost::gregorian::date(2021, boost::gregorian::Jan, 2));

   BOOST_CHECK(!query("mod.inbox", *mCore));
   BOOST_CHECK(!query("mod.calendar", *mCore));

   BOOST_CHECK(query("mod.workburden", *mCore));
   BOOST_CHECK_EQUAL(-0.2, query("mod.workburden", *mCore)->get<double>("value"));

   BOOST_CHECK(query("mod.punisher", *mCore));
   BOOST_CHECK_EQUAL(-0.15, query("mod.punisher", *mCore)->get<double>("value"));
}

BOOST_FIXTURE_TEST_CASE( TestBigCounterRewardWithModsAndChestAssigned, RewardTest ) 
{
   {
      boost::property_tree::ptree create;
      create.put("operation", "create");
      create.put("typename", "reward_modifier");
      create.put("params.value", -0.2);

      mCore->executeCommandJson(writeJson(create));
   }
   {
      boost::property_tree::ptree create;
      create.put("operation", "create");
      create.put("typename", "strategy_node");
      create.put("defined_id", "counter");
      create.put("params.typeChoice", "Counter");
      create.put("params.target", 10);
      create.put("params.reward", 100);

      expectId(mCore->executeCommandJson(writeJson(create)));
   }
   {
      boost::property_tree::ptree modify;
      modify.put("operation", "modify");
      modify.put("params.value", 10);
      modify.put("id", "counter");

      mCore->executeCommandJson(writeJson(modify));
   }

   BOOST_CHECK_EQUAL(500, queryVar("reward.points", *mCore));

   BOOST_CHECK_EQUAL(3, count(queryAll("reward_item", *mCore)));
}

static std::time_t to_time_t(const boost::gregorian::date& date )
{
	using namespace boost::posix_time;
	static ptime epoch(boost::gregorian::date(1970, 1, 1));
	time_duration::sec_type secs = (ptime(date,seconds(0)) - epoch).total_seconds();
	return std::time_t(secs);
}

BOOST_FIXTURE_TEST_CASE( ModifierExpiration, RewardTest ) 
{
   auto expirationDate = boost::gregorian::date(2021, boost::gregorian::Jan, 7);

   {
      boost::property_tree::ptree create;
      create.put("operation", "create");
      create.put("typename", "reward_modifier");
      create.put("params.value", 0.1);
      create.put("params.validUntil", to_time_t(expirationDate));
      create.put("defined_id", "expirable");

      expectId(mCore->executeCommandJson(writeJson(create)));
   }
   {
      boost::property_tree::ptree create;
      create.put("operation", "create");
      create.put("typename", "reward_modifier");
      create.put("params.value", 0.1);
      create.put("defined_id", "non_expirable");

      expectId(mCore->executeCommandJson(writeJson(create)));
   }
   mCore->onNewDay(boost::gregorian::date(2021, boost::gregorian::Jan, 1));
   BOOST_CHECK(query("expirable", *mCore));
   BOOST_CHECK(query("non_expirable", *mCore));

   mCore->onNewDay(boost::gregorian::date(2021, boost::gregorian::Jan, 2));
   BOOST_CHECK(query("expirable", *mCore));
   BOOST_CHECK(query("non_expirable", *mCore));

   mCore->onNewDay(boost::gregorian::date(2021, boost::gregorian::Jan, 9));
   BOOST_CHECK(!query("expirable", *mCore));
   BOOST_CHECK(query("non_expirable", *mCore));
}

BOOST_FIXTURE_TEST_CASE( AddPointsDebtTest, RewardTest ) 
{
   set("reward.debt", 5, *mCore);

   {
      boost::property_tree::ptree rwd;
      rwd.put("operation", "reward");
      rwd.put("points", 3);
      mCore->executeCommandJson(writeJson(rwd));

      BOOST_CHECK_EQUAL(-200, queryVar("reward.points", *mCore));

      BOOST_CHECK_EQUAL(0, query("reward.debt", *mCore)->get<double>("value"));
   }
   {
      boost::property_tree::ptree rwd;
      rwd.put("operation", "reward");
      rwd.put("points", 3);
      mCore->executeCommandJson(writeJson(rwd));

      BOOST_CHECK_EQUAL(100, queryVar("reward.points", *mCore));
      BOOST_CHECK_EQUAL(0, query("reward.debt", *mCore)->get<double>("value"));
   }
   {
      boost::property_tree::ptree rwd;
      rwd.put("operation", "reward");
      rwd.put("points", 3);
      mCore->executeCommandJson(writeJson(rwd));

      BOOST_CHECK_EQUAL(400, queryVar("reward.points", *mCore));
      BOOST_CHECK_EQUAL(0, query("reward.debt", *mCore)->get<double>("value"));
   }
}
