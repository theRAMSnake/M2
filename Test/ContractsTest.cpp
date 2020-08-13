#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <Core/ICore3.hpp>
#include "../Core/private/JsonSerializer.hpp"

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
         create.put("params.contracts.0.time", 3);
         create.put("params.contracts.0.timeGrowth", 1);
         create.put("params.contracts.0.rewardBase", 1);
         create.put("params.contracts.0.rewardPerLevel", 0.5);
         create.put("params.contracts.1.id", "id1");
         create.put("params.contracts.1.typename", "object");
         create.put("params.contracts.1.caption", "fff");
         create.put("params.contracts.1.goal", 100);
         create.put("params.contracts.1.goalGrowth", 0.1);
         create.put("params.contracts.1.time", 1);
         create.put("params.contracts.1.timeGrowth", 0);
         create.put("params.contracts.1.rewardBase", 1);
         create.put("params.contracts.1.rewardPerLevel", 3);

         mCore->executeCommandJson(writeJson(create));
      }
      {
         boost::property_tree::ptree create;
         create.put("operation", "create");
         create.put("typename", "reward_pool");
         create.put("defined_id", "pool");
         create.put("params.amount", 0);
         create.put("params.amountMax", 100);

         mCore->executeCommandJson(writeJson(create));
      }
   }

protected:

   std::shared_ptr<materia::ICore3> mCore;
};

BOOST_FIXTURE_TEST_CASE( TestCreateManual, ContractsTest ) 
{
   
}

BOOST_FIXTURE_TEST_CASE( TestNewDayEmpty, ContractsTest ) 
{
   
}

BOOST_FIXTURE_TEST_CASE( TestNewDayFull, ContractsTest ) 
{
   
}

BOOST_FIXTURE_TEST_CASE( TestNewDayCompleted, ContractsTest ) 
{
   
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

BOOST_FIXTURE_TEST_CASE( Test4Days, ContractsTest ) 
{
   
}