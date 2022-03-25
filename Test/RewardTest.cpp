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

   int getTotalCost(const boost::property_tree::ptree& p)
   {
       int result = 0;
       result += p.get<int>("costRed");
       result += p.get<int>("costBlue");
       result += p.get<int>("costPurple");
       result += p.get<int>("costYellow");
       result += p.get<int>("costGreen");

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

BOOST_FIXTURE_TEST_CASE( ShopItemAssignTest, RewardTest )
{
   {
       boost::property_tree::ptree create;
       create.put("operation", "create");
       create.put("typename", "object");
       create.put("defined_id", "config.reward");
       create.put("params.shopSize", 5);

       mCore->executeCommandJson(writeJson(create));
   }
   {
       boost::property_tree::ptree create;
       create.put("operation", "create");
       create.put("typename", "reward_shop_spawner");
       create.put("params.name", "spawner");
       create.put("params.amount", 1);
       create.put("params.weight", 0.5);
       create.put("params.cost_base", "5 7 5");
       create.put("params.type", "Assign");

       mCore->executeCommandJson(writeJson(create));
   }
   {
       boost::property_tree::ptree create;
       create.put("operation", "create");
       create.put("typename", "reward_shop_spawner");
       create.put("params.name", "spawner2");
       create.put("params.amount", 1);
       create.put("params.weight", 0.5);
       create.put("params.cost_base", "3 3");
       create.put("params.type", "Assign");

       mCore->executeCommandJson(writeJson(create));
   }

   mCore->TEST_reinitReward();

   auto all = queryAll("reward_shop_item", *mCore);
   auto ol = readJson<boost::property_tree::ptree>(all);

   std::size_t counter = 0;
   for(auto& v : ol.get_child("object_list"))
   {
      if(v.second.get<std::string>("name") == "spawner")
      {
          BOOST_CHECK_EQUAL(17, getTotalCost(v.second));
      }
      else if(v.second.get<std::string>("name") == "spawner2")
      {
          BOOST_CHECK_EQUAL(6, getTotalCost(v.second));
      }
      else
      {
          BOOST_CHECK(false);
      }
      counter++;
   }
   BOOST_CHECK_EQUAL(5, counter);
}

BOOST_FIXTURE_TEST_CASE( ShopItemListTest, RewardTest )
{
   {
       boost::property_tree::ptree create;
       create.put("operation", "create");
       create.put("typename", "object");
       create.put("defined_id", "config.reward");
       create.put("params.shopSize", 5);

       mCore->executeCommandJson(writeJson(create));
   }
   {
       boost::property_tree::ptree create;
       create.put("operation", "create");
       create.put("typename", "reward_shop_spawner");
       create.put("params.name", "spawner");
       create.put("params.amount", 1);
       create.put("params.weight", 0.5);
       create.put("params.cost_base", "5");
       create.put("params.type", "FetchFromList");
       create.put("params.listId", "lst");

       mCore->executeCommandJson(writeJson(create));
   }
   {
       boost::property_tree::ptree push;
       push.put("operation", "push");
       push.put("listId", "lst");
       push.put("value", "lst1");
       mCore->executeCommandJson(writeJson(push));
       push.put("value", "lst2");
       mCore->executeCommandJson(writeJson(push));
   }

   mCore->TEST_reinitReward();

   auto all = queryAll("reward_shop_item", *mCore);
   auto ol = readJson<boost::property_tree::ptree>(all);

   std::size_t counter = 0;
   for(auto& v : ol.get_child("object_list"))
   {
      BOOST_CHECK_EQUAL(5, getTotalCost(v.second));
      BOOST_CHECK("lst1" == v.second.get<std::string>("name") ||
              "lst2" == v.second.get<std::string>("name"));
      counter++;
   }
   BOOST_CHECK_EQUAL(5, counter);
}

BOOST_FIXTURE_TEST_CASE( BuyNotEnoughTest, RewardTest )
{
   {
      boost::property_tree::ptree create;
      create.put("operation", "create");
      create.put("typename", "object");
      create.put("defined_id", "reward.coins");
      create.put("params.Red", 3);
      create.put("params.Blue", 3);
      create.put("params.Yellow", 3);
      create.put("params.Purple", 3);
      create.put("params.Green", 3);

      expectId(mCore->executeCommandJson(writeJson(create)));
   }
   {
       boost::property_tree::ptree create;
       create.put("operation", "create");
       create.put("typename", "reward_shop_item");
       create.put("defined_id", "theItem");
       create.put("params.name", "item");
       create.put("params.amount", 1);
       create.put("params.costRed", "9");

       mCore->executeCommandJson(writeJson(create));
   }
   {
       boost::property_tree::ptree buy;
       buy.put("operation", "buyRewardItem");
       buy.put("targetId", "theItem");
       mCore->executeCommandJson(writeJson(buy));
   }

   auto coins = query("reward.coins", *mCore);
   BOOST_CHECK_EQUAL(3, coins->get<int>("Red"));
   BOOST_CHECK_EQUAL(3, coins->get<int>("Blue"));
   BOOST_CHECK_EQUAL(3, coins->get<int>("Green"));
   BOOST_CHECK_EQUAL(3, coins->get<int>("Purple"));
   BOOST_CHECK_EQUAL(3, coins->get<int>("Yellow"));

   BOOST_CHECK_EQUAL(1, count(queryAll("reward_shop_item", *mCore)));
}

BOOST_FIXTURE_TEST_CASE( BuyNonStackableTest, RewardTest )
{
   {
       boost::property_tree::ptree create;
       create.put("operation", "create");
       create.put("typename", "object");
       create.put("defined_id", "config.reward");
       create.put("params.shopSize", 5);

       mCore->executeCommandJson(writeJson(create));
   }
   {
      boost::property_tree::ptree create;
      create.put("operation", "create");
      create.put("typename", "object");
      create.put("defined_id", "reward.coins");
      create.put("params.Red", 3);
      create.put("params.Blue", 3);
      create.put("params.Yellow", 3);
      create.put("params.Purple", 3);
      create.put("params.Green", 3);

      expectId(mCore->executeCommandJson(writeJson(create)));
   }
   {
       boost::property_tree::ptree create;
       create.put("operation", "create");
       create.put("typename", "reward_shop_item");
       create.put("defined_id", "theItem");
       create.put("params.name", "item");
       create.put("params.amount", 1);
       create.put("params.costRed", "2");
       create.put("params.costBlue", "3");
       create.put("params.costYellow", "1");

       mCore->executeCommandJson(writeJson(create));
   }
   {
       boost::property_tree::ptree create;
       create.put("operation", "create");
       create.put("typename", "reward_shop_spawner");
       create.put("params.name", "spawner");
       create.put("params.amount", 1);
       create.put("params.weight", 0.5);
       create.put("params.cost_base", "5");
       create.put("params.type", "Assign");

       mCore->executeCommandJson(writeJson(create));
   }
   {
       boost::property_tree::ptree buy;
       buy.put("operation", "buyRewardItem");
       buy.put("targetId", "theItem");
       mCore->executeCommandJson(writeJson(buy));
   }

   auto coins = query("reward.coins", *mCore);
   BOOST_CHECK_EQUAL(1, coins->get<int>("Red"));
   BOOST_CHECK_EQUAL(0, coins->get<int>("Blue"));
   BOOST_CHECK_EQUAL(3, coins->get<int>("Green"));
   BOOST_CHECK_EQUAL(3, coins->get<int>("Purple"));
   BOOST_CHECK_EQUAL(2, coins->get<int>("Yellow"));

   BOOST_CHECK_EQUAL(5, count(queryAll("reward_shop_item", *mCore)));
   BOOST_CHECK(!query("theItem", *mCore));

   BOOST_CHECK_EQUAL(1, count(queryAll("reward_item", *mCore)));

   auto item = queryFirst("reward_item", *mCore);
   BOOST_CHECK_EQUAL(1, item.get<int>("amount"));
   BOOST_CHECK_EQUAL("item", item.get<std::string>("name"));
}

BOOST_FIXTURE_TEST_CASE( BuyStackableTest, RewardTest )
{
   {
       boost::property_tree::ptree create;
       create.put("operation", "create");
       create.put("typename", "object");
       create.put("defined_id", "config.reward");
       create.put("params.shopSize", 5);

       mCore->executeCommandJson(writeJson(create));
   }
   {
      boost::property_tree::ptree create;
      create.put("operation", "create");
      create.put("typename", "object");
      create.put("defined_id", "reward.coins");
      create.put("params.Red", 3);
      create.put("params.Blue", 3);
      create.put("params.Yellow", 3);
      create.put("params.Purple", 3);
      create.put("params.Green", 3);

      expectId(mCore->executeCommandJson(writeJson(create)));
   }
   {
       boost::property_tree::ptree create;
       create.put("operation", "create");
       create.put("typename", "reward_shop_item");
       create.put("defined_id", "theItem");
       create.put("params.name", "A Token");
       create.put("params.amount", 5);
       create.put("params.costRed", "1");
       create.put("params.costBlue", "1");
       create.put("params.costYellow", "1");

       mCore->executeCommandJson(writeJson(create));
   }
   {
       boost::property_tree::ptree create;
       create.put("operation", "create");
       create.put("typename", "reward_shop_spawner");
       create.put("params.name", "A Token");
       create.put("params.amount", 5);
       create.put("params.weight", 0.5);
       create.put("params.cost_base", "5");
       create.put("params.type", "Assign");

       mCore->executeCommandJson(writeJson(create));
   }
   {
       boost::property_tree::ptree buy;
       buy.put("operation", "buyRewardItem");
       buy.put("targetId", "theItem");
       mCore->executeCommandJson(writeJson(buy));
   }

   auto coins = query("reward.coins", *mCore);
   BOOST_CHECK_EQUAL(2, coins->get<int>("Red"));
   BOOST_CHECK_EQUAL(2, coins->get<int>("Blue"));
   BOOST_CHECK_EQUAL(3, coins->get<int>("Green"));
   BOOST_CHECK_EQUAL(3, coins->get<int>("Purple"));
   BOOST_CHECK_EQUAL(2, coins->get<int>("Yellow"));

   BOOST_CHECK_EQUAL(5, count(queryAll("reward_shop_item", *mCore)));
   BOOST_CHECK(!query("theItem", *mCore));

   BOOST_CHECK_EQUAL(1, count(queryAll("reward_item", *mCore)));

   auto item = queryFirst("reward_item", *mCore);
   BOOST_CHECK_EQUAL(5, item.get<int>("amount"));
   BOOST_CHECK_EQUAL("A Token", item.get<std::string>("name"));

   {
       boost::property_tree::ptree create;
       create.put("operation", "create");
       create.put("typename", "reward_shop_item");
       create.put("defined_id", "theItem");
       create.put("params.name", "A Token");
       create.put("params.amount", 8);
       create.put("params.costRed", "1");
       create.put("params.costBlue", "1");
       create.put("params.costYellow", "1");

       mCore->executeCommandJson(writeJson(create));
   }
   {
       boost::property_tree::ptree buy;
       buy.put("operation", "buyRewardItem");
       buy.put("targetId", "theItem");
       mCore->executeCommandJson(writeJson(buy));
   }
   coins = query("reward.coins", *mCore);
   BOOST_CHECK_EQUAL(1, coins->get<int>("Red"));
   BOOST_CHECK_EQUAL(1, coins->get<int>("Blue"));
   BOOST_CHECK_EQUAL(3, coins->get<int>("Green"));
   BOOST_CHECK_EQUAL(3, coins->get<int>("Purple"));
   BOOST_CHECK_EQUAL(1, coins->get<int>("Yellow"));

   BOOST_CHECK_EQUAL(5, count(queryAll("reward_shop_item", *mCore)));
   BOOST_CHECK(!query("theItem", *mCore));

   BOOST_CHECK_EQUAL(1, count(queryAll("reward_item", *mCore)));

   item = queryFirst("reward_item", *mCore);
   BOOST_CHECK_EQUAL(13, item.get<int>("amount"));
   BOOST_CHECK_EQUAL("A Token", item.get<std::string>("name"));
   {
       boost::property_tree::ptree create;
       create.put("operation", "create");
       create.put("typename", "reward_shop_item");
       create.put("defined_id", "theItem");
       create.put("params.name", "B Token");
       create.put("params.amount", 8);
       create.put("params.costRed", "1");
       create.put("params.costBlue", "1");
       create.put("params.costYellow", "1");

       mCore->executeCommandJson(writeJson(create));
   }
   {
       boost::property_tree::ptree buy;
       buy.put("operation", "buyRewardItem");
       buy.put("targetId", "theItem");
       mCore->executeCommandJson(writeJson(buy));
   }
   BOOST_CHECK_EQUAL(2, count(queryAll("reward_item", *mCore)));
}
