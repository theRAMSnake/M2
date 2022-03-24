#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <Core/ICore3.hpp>
#include "../Core/private/JsonSerializer.hpp"
#include "Utils.hpp"

class ContractsTest
{
public:
   ContractsTest()
   {
      system("rm Test.db");
      mCore = materia::createCore({"Test.db"});
      set("work.burden", 500, *mCore);

      {
         boost::property_tree::ptree create;
         create.put("operation", "create");
         create.put("typename", "object");
         create.put("defined_id", "config.reward");
         create.put("params.contracts.id", "contracts");
         create.put("params.contracts.typename", "object");
         create.put("params.contracts.0.id", "id0");
         create.put("params.contracts.0.typename", "object");
         create.put("params.contracts.0.caption", "ddd % ddd");
         create.put("params.contracts.0.goal", 10);
         create.put("params.contracts.0.goalGrowth", 1);
         create.put("params.contracts.0.time", 5);
         create.put("params.contracts.0.timeGrowth", 1);
         create.put("params.contracts.0.rewardBase", 1);
         create.put("params.contracts.0.rewardPerLevel", 0.5);
         create.put("params.contracts.0.reward_variable", "");
         create.put("params.contracts.0.reward_color", "Green");
         create.put("params.contracts.1.id", "id1");
         create.put("params.contracts.1.typename", "object");
         create.put("params.contracts.1.caption", "fff");
         create.put("params.contracts.1.goal", 100);
         create.put("params.contracts.1.goalGrowth", 0.1);
         create.put("params.contracts.1.time", 5);
         create.put("params.contracts.1.timeGrowth", 0);
         create.put("params.contracts.1.rewardBase", 1);
         create.put("params.contracts.1.rewardPerLevel", 3);
         create.put("params.contracts.1.reward_variable", "");
         create.put("params.contracts.1.reward_color", "Blue");

         expectId(mCore->executeCommandJson(writeJson(create)));
      }
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
       }
   }

protected:

   std::shared_ptr<materia::ICore3> mCore;
};

BOOST_FIXTURE_TEST_CASE( TestNewDayEmpty, ContractsTest ) 
{
   mCore->onNewDay(boost::gregorian::day_clock::local_day());

   BOOST_CHECK_EQUAL(1, count(queryAll("reward_contract", *mCore)));
}

BOOST_FIXTURE_TEST_CASE( TestNewDayFull, ContractsTest )
{
   mCore->onNewDay(boost::gregorian::day_clock::local_day());

   BOOST_CHECK_EQUAL(1, count(queryAll("reward_contract", *mCore)));

   mCore->onNewDay(boost::gregorian::day_clock::local_day());

   BOOST_CHECK_EQUAL(2, count(queryAll("reward_contract", *mCore)));

   mCore->onNewDay(boost::gregorian::day_clock::local_day());

   BOOST_CHECK_EQUAL(2, count(queryAll("reward_contract", *mCore)));
}

BOOST_FIXTURE_TEST_CASE( TestNewDayCompleted, ContractsTest )
{
   mCore->onNewDay(boost::gregorian::day_clock::local_day());

   auto c = queryFirst("reward_contract", *mCore);

   auto id = c.get<std::string>("id");
   auto configId = c.get<std::string>("config_id");

   boost::property_tree::ptree modify;
   modify.put("operation", "modify");
   modify.put("params.score", 10000);
   modify.put("id", id);

   mCore->executeCommandJson(writeJson(modify));

   mCore->onNewDay(boost::gregorian::day_clock::local_day());

   //Expect removed
   auto cont = query(id, *mCore);
   BOOST_CHECK(!cont);

   //Expect coins added
   auto coins = query("reward.coins", *mCore);
   BOOST_CHECK_EQUAL(0, coins->get<int>("Red"));
   BOOST_CHECK(1 == coins->get<int>("Blue") || 1 == coins->get<int>("Green"));
   BOOST_CHECK_EQUAL(0, coins->get<int>("Purple"));
   BOOST_CHECK_EQUAL(0, coins->get<int>("Yellow"));

   //Expect level raised
   auto conf = query("reward.cb", *mCore);
   BOOST_CHECK_EQUAL(1, conf->get<int>(configId));
}

BOOST_FIXTURE_TEST_CASE( TestNewDayCompletedFull, ContractsTest ) 
{
   mCore->onNewDay(boost::gregorian::day_clock::local_day());
   mCore->onNewDay(boost::gregorian::day_clock::local_day());
   mCore->onNewDay(boost::gregorian::day_clock::local_day());

   auto c = queryFirst("reward_contract", *mCore);

   auto id = c.get<std::string>("id");

   boost::property_tree::ptree modify;
   modify.put("operation", "modify");
   modify.put("params.score", 10000);
   modify.put("id", id);

   mCore->executeCommandJson(writeJson(modify));

   mCore->onNewDay(boost::gregorian::day_clock::local_day());

   BOOST_CHECK_EQUAL(2, count(queryAll("reward_contract", *mCore)));
}

BOOST_FIXTURE_TEST_CASE( TestLeveledCreate, ContractsTest ) 
{
   //Complete 10 contracts
   for(int i = 0; i < 10; ++i)
   {
      mCore->onNewDay(boost::gregorian::day_clock::local_day());

      auto c = queryFirst("reward_contract", *mCore);

      auto id = c.get<std::string>("id");
      
      boost::property_tree::ptree modify;
      modify.put("operation", "modify");
      modify.put("params.score", 10000);
      modify.put("id", id);

      mCore->executeCommandJson(writeJson(modify));
   }

   mCore->onNewDay(boost::gregorian::day_clock::local_day());

   //Check levels
   auto conf = query("reward.cb", *mCore);
   BOOST_CHECK_EQUAL(10, conf->get<int>("id0") + conf->get<int>("id1"));

   //Check leveled create
   auto c = queryFirst("reward_contract", *mCore);
   auto confId = c.get<std::string>("config_id");
   auto level = conf->get<int>(confId);

   if(confId == "id0")
   {
      BOOST_CHECK_EQUAL(static_cast<int>(std::round(1 + 0.5 * level)), c.get<int>("reward"));
      BOOST_CHECK_EQUAL(10 + 1 * level, c.get<int>("goal"));
      BOOST_CHECK_EQUAL(5 + 1 * level, c.get<int>("daysLeft"));
   }
   else
   {
      BOOST_CHECK_EQUAL(1 + 3 * level, c.get<int>("reward"));
      BOOST_CHECK_EQUAL(std::round(100 + 0.1 * level), c.get<int>("goal"));
      BOOST_CHECK_EQUAL(5, c.get<int>("daysLeft"));
   }
}

BOOST_FIXTURE_TEST_CASE( TestExpiration, ContractsTest ) 
{
   set("reward.points", 1, *mCore);
   mCore->onNewDay(boost::gregorian::day_clock::local_day());
   auto c = queryFirst("reward_contract", *mCore);
   auto id = c.get<std::string>("id");
   auto exp = c.get<int>("daysLeft");

   while(true)
   {
      exp--;
      mCore->onNewDay(boost::gregorian::day_clock::local_day());

      auto cont = query(id, *mCore);
      if(exp == 0)
      {
         BOOST_CHECK(!cont);
         break;
      }
      else
      {
         BOOST_CHECK_EQUAL(exp, cont->get<int>("daysLeft"));
      }
   }

   //Check no points is awarded
   BOOST_CHECK_EQUAL(1, queryVar("reward.points", *mCore));

   //Check we still have 2 contracts
   BOOST_CHECK_EQUAL(2, count(queryAll("reward_contract", *mCore)));
}

BOOST_FIXTURE_TEST_CASE( Test40Days, ContractsTest )
{
   for(int i = 0; i < 40; ++i)
   {
      mCore->onNewDay(boost::gregorian::day_clock::local_day());
   }
}

BOOST_FIXTURE_TEST_CASE( TestVarContract, ContractsTest )
{
   {
      auto config = query("config.reward", *mCore);

      BOOST_CHECK(config);

      auto& c = *config;

      c.put("contracts.2.id", "id2");
      c.put("contracts.2.typename", "object");
      c.put("contracts.2.caption", "fff");
      c.put("contracts.2.goal", 100);
      c.put("contracts.2.goalGrowth", 0);
      c.put("contracts.2.time", 5);
      c.put("contracts.2.timeGrowth", 0);
      c.put("contracts.2.rewardBase", 1);
      c.put("contracts.2.rewardPerLevel", 0);
      c.put("contracts.2.reward_color", "");
      c.put("contracts.2.reward_variable", "testvar");

      boost::property_tree::ptree modify;
      modify.put("operation", "modify");
      modify.put_child("params", c);
      modify.put("id", "config.reward");
   }

   set("testvar", 0, *mCore);

   int boostAmount = 0;
   for(int i = 0; i < 200; ++i)
   {
      mCore->onNewDay(boost::gregorian::day_clock::local_day());

      auto contrt = query("reward_contract", *mCore);
      if(contrt)
      {
         auto& c = *contrt;

         auto id = c.get<std::string>("id");
         auto var_id = c.get<std::string>("reward_variable");

         if(var_id != "testvar")
         {
            boost::property_tree::ptree destroy;
            destroy.put("operation", "destroy");
            destroy.put("id", id);
            mCore->executeCommandJson(writeJson(destroy));
         }
         else
         {
            boost::property_tree::ptree modify;
            modify.put("operation", "modify");
            modify.put("params.score", 10000);
            modify.put("id", id);

            mCore->executeCommandJson(writeJson(modify));

            boostAmount += 1;
         }
      }
   }

   auto var = query("testvar", *mCore);

   BOOST_CHECK(var);
   BOOST_CHECK_EQUAL(boostAmount, var->get<int>("value"));
}
