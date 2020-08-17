#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE a
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
         create.put("params.contracts.1.id", "id1");
         create.put("params.contracts.1.typename", "object");
         create.put("params.contracts.1.caption", "fff");
         create.put("params.contracts.1.goal", 100);
         create.put("params.contracts.1.goalGrowth", 0.1);
         create.put("params.contracts.1.time", 5);
         create.put("params.contracts.1.timeGrowth", 0);
         create.put("params.contracts.1.rewardBase", 1);
         create.put("params.contracts.1.rewardPerLevel", 3);

         expectId(mCore->executeCommandJson(writeJson(create)));
      }
      {
         boost::property_tree::ptree create;
         create.put("operation", "create");
         create.put("typename", "reward_pool");
         create.put("defined_id", "pool");
         create.put("params.amount", 0);
         create.put("params.amountMax", 100);

         expectId(mCore->executeCommandJson(writeJson(create)));
      }
      {
         //Add inbox to prevent spontaneoues points
         std::string inboxFill = "{\"operation\":\"create\","
            "\"typename\":\"simple_list\","
            "\"defined_id\":\"inbox\","
            "\"params\":{\"objects\":[\"ddd\"]}}";
         expectId(mCore->executeCommandJson(inboxFill));
      }
   }

protected:

   std::shared_ptr<materia::ICore3> mCore;
};

BOOST_FIXTURE_TEST_CASE( TestNewDayEmpty, ContractsTest ) 
{
   mCore->onNewDay();

   BOOST_CHECK_EQUAL(1, count(queryAll("reward_contract", *mCore)));
}

BOOST_FIXTURE_TEST_CASE( TestNewDayFull, ContractsTest ) 
{
   mCore->onNewDay();

   BOOST_CHECK_EQUAL(1, count(queryAll("reward_contract", *mCore)));

   mCore->onNewDay();

   BOOST_CHECK_EQUAL(2, count(queryAll("reward_contract", *mCore)));

   mCore->onNewDay();

   BOOST_CHECK_EQUAL(3, count(queryAll("reward_contract", *mCore)));

   mCore->onNewDay();

   BOOST_CHECK_EQUAL(3, count(queryAll("reward_contract", *mCore)));
}

BOOST_FIXTURE_TEST_CASE( TestNewDayCompleted, ContractsTest ) 
{
   mCore->onNewDay();

   auto c = queryFirst("reward_contract", *mCore);

   auto expectedReward = c.get<int>("reward");
   auto id = c.get<std::string>("id");
   auto configId = c.get<std::string>("config_id");
   
   boost::property_tree::ptree modify;
   modify.put("operation", "modify");
   modify.put("params.score", 10000);
   modify.put("id", id);

   mCore->executeCommandJson(writeJson(modify));

   mCore->onNewDay();

   //Expect removed
   auto cont = query(id, *mCore);
   BOOST_CHECK(!cont);

   //Expect points added
   auto p = queryFirst("reward_pool", *mCore);
   BOOST_CHECK_EQUAL(expectedReward, p.get<int>("amount"));

   //Expect level raised
   auto conf = query("reward.cb", *mCore);
   BOOST_CHECK_EQUAL(1, conf->get<int>(configId));
}

BOOST_FIXTURE_TEST_CASE( TestNewDayCompletedFull, ContractsTest ) 
{
   mCore->onNewDay();
   mCore->onNewDay();
   mCore->onNewDay();

   auto c = queryFirst("reward_contract", *mCore);

   auto id = c.get<std::string>("id");

   boost::property_tree::ptree modify;
   modify.put("operation", "modify");
   modify.put("params.score", 10000);
   modify.put("id", id);

   mCore->executeCommandJson(writeJson(modify));

   mCore->onNewDay();

   BOOST_CHECK_EQUAL(3, count(queryAll("reward_contract", *mCore)));
}

BOOST_FIXTURE_TEST_CASE( TestLeveledCreate, ContractsTest ) 
{
   //Complete 10 contracts
   for(int i = 0; i < 10; ++i)
   {
      mCore->onNewDay();

      auto c = queryFirst("reward_contract", *mCore);

      auto id = c.get<std::string>("id");
      
      boost::property_tree::ptree modify;
      modify.put("operation", "modify");
      modify.put("params.score", 10000);
      modify.put("id", id);

      mCore->executeCommandJson(writeJson(modify));
   }

   mCore->onNewDay();

   //Check levels
   auto conf = query("reward.cb", *mCore);
   BOOST_CHECK_EQUAL(10, conf->get<int>("id0") + conf->get<int>("id1"));

   //Check leveled create
   auto c = queryFirst("reward_contract", *mCore);
   auto confId = c.get<std::string>("config_id");
   auto level = conf->get<int>(confId);

   if(confId == "id0")
   {
      BOOST_CHECK_EQUAL(1 + 0.5 * level, c.get<int>("reward"));
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
   mCore->onNewDay();
   auto c = queryFirst("reward_contract", *mCore);
   auto id = c.get<std::string>("id");
   auto exp = c.get<int>("daysLeft");

   while(true)
   {
      exp--;
      mCore->onNewDay();

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
   auto p = queryFirst("reward_pool", *mCore);
   BOOST_CHECK_EQUAL(0, p.get<int>("amount"));

   //Check we still have 3 contracts
   BOOST_CHECK_EQUAL(3, count(queryAll("reward_contract", *mCore)));
}

BOOST_FIXTURE_TEST_CASE( Test40Days, ContractsTest ) 
{
   for(int i = 0; i < 40; ++i)
   {
      mCore->onNewDay();
   }
}