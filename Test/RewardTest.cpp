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
   int getTotalCoinAmount()
   {
       auto coins = query("reward.coins", *mCore);
       int result = 0;
       result += coins->get<int>("Red");
       result += coins->get<int>("Blue");
       result += coins->get<int>("Yellow");
       result += coins->get<int>("Green");
       result += coins->get<int>("Purple");

       return result;
   }

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

BOOST_FIXTURE_TEST_CASE( GeneratorsTestRandom, RewardTest )
{
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

   {
      boost::property_tree::ptree create;
      create.put("operation", "create");
      create.put("typename", "object");
      create.put("defined_id", "reward.coins");
      create.put("params.Red", 0);
      create.put("params.Blue", 0);
      create.put("params.Yellow", 0);
      create.put("params.Purple", 0);
      create.put("params.Green", 0);

      expectId(mCore->executeCommandJson(writeJson(create)));
   }

   {
      boost::property_tree::ptree create;
      create.put("operation", "create");
      create.put("typename", "reward_generator");
      create.put("params.value", 1);
      create.put("params.type", "Random");

      expectId(mCore->executeCommandJson(writeJson(create)));
   }

   mCore->onNewDay(boost::gregorian::date(2021, boost::gregorian::Jan, 1));

   BOOST_CHECK_EQUAL(1 + 1, getTotalCoinAmount());

   mCore->onNewDay(boost::gregorian::date(2021, boost::gregorian::Jan, 2));

   BOOST_CHECK_EQUAL(2 + 2, getTotalCoinAmount());

   mCore->onNewDay(boost::gregorian::date(2021, boost::gregorian::Jan, 3));

   BOOST_CHECK_EQUAL(3 + 3, getTotalCoinAmount());

   {
      boost::property_tree::ptree create;
      create.put("operation", "create");
      create.put("typename", "reward_generator");
      create.put("params.value", 1);
      create.put("params.type", "Random");

      expectId(mCore->executeCommandJson(writeJson(create)));
   }

   mCore->onNewDay(boost::gregorian::date(2021, boost::gregorian::Jan, 4));

   BOOST_CHECK_EQUAL(5 + 4, getTotalCoinAmount());

   deleteAll("reward_generator", *mCore);

   {
      boost::property_tree::ptree create;
      create.put("operation", "create");
      create.put("typename", "reward_generator");
      create.put("params.value", -100);
      create.put("params.type", "Random");

      expectId(mCore->executeCommandJson(writeJson(create)));
   }
   set("work.burden", 500, *mCore);

}

BOOST_FIXTURE_TEST_CASE( GeneratorsTestSpecific, RewardTest )
{
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
   {
      boost::property_tree::ptree create;
      create.put("operation", "create");
      create.put("typename", "object");
      create.put("defined_id", "reward.coins");
      create.put("params.Red", 0);
      create.put("params.Blue", 0);
      create.put("params.Yellow", 0);
      create.put("params.Purple", 0);
      create.put("params.Green", 0);

      expectId(mCore->executeCommandJson(writeJson(create)));
   }
   {
      boost::property_tree::ptree create;
      create.put("operation", "create");
      create.put("typename", "reward_generator");
      create.put("params.value", 1);
      create.put("params.type", "Specific");
      create.put("params.color", "Red");

      expectId(mCore->executeCommandJson(writeJson(create)));
   }
   {
      boost::property_tree::ptree create;
      create.put("operation", "create");
      create.put("typename", "reward_generator");
      create.put("params.value", 2);
      create.put("params.type", "Specific");
      create.put("params.color", "Blue");

      expectId(mCore->executeCommandJson(writeJson(create)));
   }

   mCore->onNewDay(boost::gregorian::date(2021, boost::gregorian::Jan, 4));
   auto coins = query("reward.coins", *mCore);
   BOOST_CHECK_EQUAL(1, coins->get<int>("Red"));
   BOOST_CHECK_EQUAL(2, coins->get<int>("Blue"));
   BOOST_CHECK_EQUAL(0, coins->get<int>("Green"));
   BOOST_CHECK_EQUAL(0, coins->get<int>("Purple"));
   BOOST_CHECK_EQUAL(0, coins->get<int>("Yellow"));

   mCore->onNewDay(boost::gregorian::date(2021, boost::gregorian::Jan, 5));
   coins = query("reward.coins", *mCore);
   BOOST_CHECK_EQUAL(2, coins->get<int>("Red"));
   BOOST_CHECK_EQUAL(4, coins->get<int>("Blue"));
   BOOST_CHECK_EQUAL(0, coins->get<int>("Green"));
   BOOST_CHECK_EQUAL(0, coins->get<int>("Purple"));
   BOOST_CHECK_EQUAL(0, coins->get<int>("Yellow"));

   {
      boost::property_tree::ptree create;
      create.put("operation", "create");
      create.put("typename", "reward_generator");
      create.put("params.value", -5);
      create.put("params.type", "Specific");
      create.put("params.color", "Blue");

      expectId(mCore->executeCommandJson(writeJson(create)));
   }
   {
      boost::property_tree::ptree create;
      create.put("operation", "create");
      create.put("typename", "reward_generator");
      create.put("params.value", -5);
      create.put("params.type", "Specific");
      create.put("params.color", "Green");

      expectId(mCore->executeCommandJson(writeJson(create)));
   }

   mCore->onNewDay(boost::gregorian::date(2021, boost::gregorian::Jan, 6));
   coins = query("reward.coins", *mCore);
   BOOST_CHECK_EQUAL(3, coins->get<int>("Red"));
   BOOST_CHECK(coins->get<int>("Blue") < 3);
   BOOST_CHECK_EQUAL(0, coins->get<int>("Green"));
   BOOST_CHECK_EQUAL(0, coins->get<int>("Purple"));
   BOOST_CHECK_EQUAL(0, coins->get<int>("Yellow"));

   mCore->onNewDay(boost::gregorian::date(2021, boost::gregorian::Jan, 7));
   coins = query("reward.coins", *mCore);
   BOOST_CHECK_EQUAL(4, coins->get<int>("Red"));
   BOOST_CHECK(coins->get<int>("Blue") < 3);
   BOOST_CHECK_EQUAL(0, coins->get<int>("Green"));
   BOOST_CHECK_EQUAL(0, coins->get<int>("Purple"));
   BOOST_CHECK_EQUAL(0, coins->get<int>("Yellow"));
}
