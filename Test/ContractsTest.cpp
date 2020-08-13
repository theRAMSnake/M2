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
   auto c = queryFirst("reward_contract", *mCore));

   auto expectedReward = c.get<int>("reward");

   boost::property_tree::ptree modify;
   modify.put("operation", "modify");
   modify.put("params.score", 10000);
   modify.put("id", c.get<std::string>("id"));

   mCore->executeCommandJson(writeJson(modify));

   mCore->onNewDay();

   //Expect removed
   //Expect points added
   //Expect level raised
}

BOOST_FIXTURE_TEST_CASE( TestNewDayCompletedFull, ContractsTest ) 
{
   
}

BOOST_FIXTURE_TEST_CASE( TestLeveledCreate, ContractsTest ) 
{
   
}

BOOST_FIXTURE_TEST_CASE( TestExpiration, ContractsTest ) 
{
   
}

BOOST_FIXTURE_TEST_CASE( Test40Days, ContractsTest ) 
{
   for(int i = 0; i < 40; ++i)
   {
      mCore->onNewDay();
   }
}