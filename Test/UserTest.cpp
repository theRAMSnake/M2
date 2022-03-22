#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <Core/ICore3.hpp>
#include "../Core/private/JsonSerializer.hpp"
#include "Utils.hpp"

class UserTest
{
public:
   UserTest() 
   {
      system("rm Test.db");
      mCore = materia::createCore({"Test.db"});

       mCore = materia::createCore({"Test.db"});
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
   }

protected:

   std::shared_ptr<materia::ICore3> mCore;
};

BOOST_FIXTURE_TEST_CASE(CompleteGoalNode, UserTest)
{
   //Create goal node
   {
      boost::property_tree::ptree create;
      create.put("operation", "create");
      create.put("typename", "strategy_node");
      create.put("defined_id", "g");
      create.put("params.typeChoice", "Task");
      create.put("params.reward", 1);

      expectId(mCore->executeCommandJson(writeJson(create)));
   }
    {
        boost::property_tree::ptree create;
        create.put("operation", "create");
        create.put("typename", "core_value");
        create.put("defined_id", "cv");
        create.put("params.color", "Yellow");

        expectId(mCore->executeCommandJson(writeJson(create)));
    }
    {
        boost::property_tree::ptree create;
        create.put("operation", "create");
        create.put("typename", "connection");
        create.put("params.A", "g");
        create.put("params.B", "cv");
        create.put("params.type", "Reference");

        expectId(mCore->executeCommandJson(writeJson(create)));
    }
   //Create reference
   {
      boost::property_tree::ptree create;
      create.put("operation", "create");
      create.put("typename", "calendar_item");
      create.put("defined_id", "c");
      create.put("params.entityTypeChoice", "StrategyNodeReference");

      expectId(mCore->executeCommandJson(writeJson(create)));

      boost::property_tree::ptree createLink;
      createLink.put("operation", "create");
      createLink.put("typename", "connection");
      createLink.put("params.A", "c");
      createLink.put("params.B", "g");
      createLink.put("params.type", "Reference");
      expectId(mCore->executeCommandJson(writeJson(createLink)));
   }
   //Complete reference
   {
      boost::property_tree::ptree complete;
      complete.put("operation", "complete");
      complete.put("id", "c");

      mCore->executeCommandJson(writeJson(complete));
   }
   //Check goal node
   auto g = queryFirst("strategy_node", *mCore);
   BOOST_CHECK(g.get<bool>("isAchieved"));

    auto coins = query("reward.coins", *mCore);
    BOOST_CHECK_EQUAL(0, coins->get<int>("Red"));
    BOOST_CHECK_EQUAL(0, coins->get<int>("Blue"));
    BOOST_CHECK_EQUAL(0, coins->get<int>("Green"));
    BOOST_CHECK_EQUAL(0, coins->get<int>("Purple"));
    BOOST_CHECK_EQUAL(1, coins->get<int>("Yellow"));
}

BOOST_FIXTURE_TEST_CASE(CompleteCounterStep, UserTest)
{
   set("reward.points", 0, *mCore);
   //Create counter node
   {
      boost::property_tree::ptree create;
      create.put("operation", "create");
      create.put("typename", "strategy_node");
      create.put("defined_id", "g");
      create.put("params.typeChoice", "Counter");
      create.put("params.target", 2);
      create.put("params.reward", 1);

      expectId(mCore->executeCommandJson(writeJson(create)));
   }
    {
        boost::property_tree::ptree create;
        create.put("operation", "create");
        create.put("typename", "core_value");
        create.put("defined_id", "cv");
        create.put("params.color", "Yellow");

        expectId(mCore->executeCommandJson(writeJson(create)));
    }
    {
        boost::property_tree::ptree create;
        create.put("operation", "create");
        create.put("typename", "connection");
        create.put("params.A", "g");
        create.put("params.B", "cv");
        create.put("params.type", "Reference");

        expectId(mCore->executeCommandJson(writeJson(create)));
    }
   //Create reference
   {
      boost::property_tree::ptree create;
      create.put("operation", "create");
      create.put("typename", "calendar_item");
      create.put("defined_id", "c");
      create.put("params.entityTypeChoice", "StrategyNodeReference");

      expectId(mCore->executeCommandJson(writeJson(create)));

      boost::property_tree::ptree createLink;
      createLink.put("operation", "create");
      createLink.put("typename", "connection");
      createLink.put("params.A", "c");
      createLink.put("params.B", "g");
      createLink.put("params.type", "Reference");
      expectId(mCore->executeCommandJson(writeJson(createLink)));
   }
   //Complete reference
   {
      boost::property_tree::ptree complete;
      complete.put("operation", "complete");
      complete.put("id", "c");

      mCore->executeCommandJson(writeJson(complete));
   }
   //Check counter node
   auto g = queryFirst("strategy_node", *mCore);
   BOOST_CHECK(!g.get<bool>("isAchieved"));
   BOOST_CHECK_EQUAL(1, g.get<int>("value"));

    auto coins = query("reward.coins", *mCore);
    BOOST_CHECK_EQUAL(0, coins->get<int>("Red"));
    BOOST_CHECK_EQUAL(0, coins->get<int>("Blue"));
    BOOST_CHECK_EQUAL(0, coins->get<int>("Green"));
    BOOST_CHECK_EQUAL(0, coins->get<int>("Purple"));
    BOOST_CHECK_EQUAL(0, coins->get<int>("Yellow"));
}

BOOST_FIXTURE_TEST_CASE(CompleteCounterFull, UserTest)
{
   //Create counter node
   {
      boost::property_tree::ptree create;
      create.put("operation", "create");
      create.put("typename", "strategy_node");
      create.put("defined_id", "g");
      create.put("params.typeChoice", "Counter");
      create.put("params.target", 1);
      create.put("params.reward", 30);

      expectId(mCore->executeCommandJson(writeJson(create)));
   }
    {
        boost::property_tree::ptree create;
        create.put("operation", "create");
        create.put("typename", "core_value");
        create.put("defined_id", "cv");
        create.put("params.color", "Yellow");

        expectId(mCore->executeCommandJson(writeJson(create)));
    }
    {
        boost::property_tree::ptree create;
        create.put("operation", "create");
        create.put("typename", "connection");
        create.put("params.A", "g");
        create.put("params.B", "cv");
        create.put("params.type", "Reference");

        expectId(mCore->executeCommandJson(writeJson(create)));
    }
   //Create reference
   {
      boost::property_tree::ptree create;
      create.put("operation", "create");
      create.put("typename", "calendar_item");
      create.put("defined_id", "c");
      create.put("params.entityTypeChoice", "StrategyNodeReference");

      expectId(mCore->executeCommandJson(writeJson(create)));

      boost::property_tree::ptree createLink;
      createLink.put("operation", "create");
      createLink.put("typename", "connection");
      createLink.put("params.A", "c");
      createLink.put("params.B", "g");
      createLink.put("params.type", "Reference");
      expectId(mCore->executeCommandJson(writeJson(createLink)));
   }
   //Complete reference
   {
      boost::property_tree::ptree complete;
      complete.put("operation", "complete");
      complete.put("id", "c");

      mCore->executeCommandJson(writeJson(complete));
   }
   //Check counter node
   auto g = queryFirst("strategy_node", *mCore);
   BOOST_CHECK(g.get<bool>("isAchieved"));
   BOOST_CHECK_EQUAL(1, g.get<int>("value"));

    auto coins = query("reward.coins", *mCore);
    BOOST_CHECK_EQUAL(0, coins->get<int>("Red"));
    BOOST_CHECK_EQUAL(0, coins->get<int>("Blue"));
    BOOST_CHECK_EQUAL(0, coins->get<int>("Green"));
    BOOST_CHECK_EQUAL(0, coins->get<int>("Purple"));
    BOOST_CHECK_EQUAL(30, coins->get<int>("Yellow"));
}

BOOST_FIXTURE_TEST_CASE(AdvanceCalendar, UserTest)
{
   boost::property_tree::ptree create;
   create.put("operation", "create");
   create.put("typename", "calendar_item");
   create.put("params.text", "a");
   create.put("params.timestamp", 80000/*outdated*/);
   create.put("params.entityTypeChoice", "Task");

   expectId(mCore->executeCommandJson(writeJson(create)));

   create.put("params.entityTypeChoice", "Event");

   expectId(mCore->executeCommandJson(writeJson(create)));

   create.put("params.timestamp", time(0) + 100000);
   create.put("params.entityTypeChoice", "Task");

   expectId(mCore->executeCommandJson(writeJson(create)));

   create.put("params.timestamp", time(0) + 500000);

   expectId(mCore->executeCommandJson(writeJson(create)));

   mCore->onNewDay(boost::gregorian::day_clock::local_day());

   BOOST_CHECK_EQUAL(4, count(queryAll("calendar_item", *mCore)));
   BOOST_CHECK_EQUAL(1, count(queryCondition("calendar_item", ".timestamp < " + std::to_string(time(0)), *mCore)));
}

