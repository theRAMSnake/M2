#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <Core/ICore3.hpp>
#include "../Core/private/JsonSerializer.hpp"
#include "Utils.hpp"
#include <ctime>

class StrategyTest
{
public:
   StrategyTest()
   {
       system("rm Test.db");
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

BOOST_FIXTURE_TEST_CASE( UpdateWaitNode, StrategyTest ) 
{
    boost::property_tree::ptree create;
    create.put("operation", "create");
    create.put("typename", "strategy_node");
    create.put("defined_id", "w1");
    create.put("params.typeChoice", "Wait");
    create.put("params.date", 100);

    expectId(mCore->executeCommandJson(writeJson(create)));

    create.put("defined_id", "w2");
    create.put("params.date", time(0) + 31557600);

    expectId(mCore->executeCommandJson(writeJson(create)));

    mCore->onNewDay(boost::gregorian::day_clock::local_day());

    auto w1 = query("w1", *mCore);
    auto w2 = query("w2", *mCore);

    BOOST_CHECK(w1->get<bool>("isAchieved"));
    BOOST_CHECK(!w2->get<bool>("isAchieved"));
}

BOOST_FIXTURE_TEST_CASE( CleanupDeletedNode, StrategyTest ) 
{
    boost::property_tree::ptree create;
    create.put("operation", "create");
    create.put("typename", "strategy_node");
    create.put("params.type", 0);
    create.put("defined_id", "n1");

    expectId(mCore->executeCommandJson(writeJson(create)));

    create.put("defined_id", "n2");

    expectId(mCore->executeCommandJson(writeJson(create)));

    create.put("defined_id", "subject");

    expectId(mCore->executeCommandJson(writeJson(create)));

    for(int i = 0; i < 3; ++i)
    {
        create.put("defined_id", "child" + std::to_string(i));
        expectId(mCore->executeCommandJson(writeJson(create)));

        boost::property_tree::ptree createLink;
        createLink.put("operation", "create");
        createLink.put("typename", "connection");
        createLink.put("params.A", "subject");
        createLink.put("params.B", "child" + std::to_string(i));
        createLink.put("params.type", "Hierarchy");
        expectId(mCore->executeCommandJson(writeJson(createLink)));
    }

    boost::property_tree::ptree createLink;
    createLink.put("operation", "create");
    createLink.put("typename", "connection");
    createLink.put("params.A", "n1");
    createLink.put("params.B", "n2");
    createLink.put("params.type", "Requirement");
    expectId(mCore->executeCommandJson(writeJson(createLink)));

    createLink.put("params.A", "n1");
    createLink.put("params.B", "subject");
    expectId(mCore->executeCommandJson(writeJson(createLink)));

    createLink.put("params.A", "subject");
    createLink.put("params.B", "n2");
    expectId(mCore->executeCommandJson(writeJson(createLink)));

    boost::property_tree::ptree destroy;
    destroy.put("operation", "destroy");
    destroy.put("id", "subject");

    mCore->executeCommandJson(writeJson(destroy));

    BOOST_CHECK_EQUAL(2, count(queryAll("strategy_node", *mCore)));

    BOOST_CHECK(query("n1", *mCore));
    BOOST_CHECK(query("n2", *mCore));
}

BOOST_FIXTURE_TEST_CASE( CounterIsAchievedCalculation, StrategyTest ) 
{
    boost::property_tree::ptree create;
    create.put("operation", "create");
    create.put("typename", "strategy_node");
    create.put("defined_id", "counter");
    create.put("params.typeChoice", "Counter");
    create.put("params.target", 10);

    expectId(mCore->executeCommandJson(writeJson(create)));

    auto c = query("counter", *mCore);
    BOOST_CHECK(!c->get<bool>("isAchieved"));

    boost::property_tree::ptree modify;
    modify.put("operation", "modify");
    modify.put("params.value", 10);
    modify.put("id", "counter");

    mCore->executeCommandJson(writeJson(modify));

    c = query("counter", *mCore);
    BOOST_CHECK(c->get<bool>("isAchieved"));
}

BOOST_FIXTURE_TEST_CASE( RewardingNoCoreRef, StrategyTest )
{
    {
        boost::property_tree::ptree create;
        create.put("operation", "create");
        create.put("typename", "strategy_node");
        create.put("defined_id", "g");
        create.put("params.type", 0);
        create.put("params.reward", 10);

        expectId(mCore->executeCommandJson(writeJson(create)));
    }

    boost::property_tree::ptree modify;
    modify.put("operation", "modify");
    modify.put("params.isAchieved", true);
    modify.put("id", "g");

    mCore->executeCommandJson(writeJson(modify));

    auto coins = query("reward.coins", *mCore);
    BOOST_CHECK_EQUAL(0, coins->get<int>("Red"));
    BOOST_CHECK_EQUAL(0, coins->get<int>("Blue"));
    BOOST_CHECK_EQUAL(0, coins->get<int>("Green"));
    BOOST_CHECK_EQUAL(0, coins->get<int>("Purple"));
    BOOST_CHECK_EQUAL(0, coins->get<int>("Yellow"));
}

BOOST_FIXTURE_TEST_CASE( RewardingWithCoreRef, StrategyTest )
{
    {
        boost::property_tree::ptree create;
        create.put("operation", "create");
        create.put("typename", "strategy_node");
        create.put("defined_id", "g");
        create.put("params.type", 0);
        create.put("params.reward", 10);

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

    boost::property_tree::ptree modify;
    modify.put("operation", "modify");
    modify.put("params.isAchieved", true);
    modify.put("id", "g");

    mCore->executeCommandJson(writeJson(modify));

    auto coins = query("reward.coins", *mCore);
    BOOST_CHECK_EQUAL(0, coins->get<int>("Red"));
    BOOST_CHECK_EQUAL(0, coins->get<int>("Blue"));
    BOOST_CHECK_EQUAL(0, coins->get<int>("Green"));
    BOOST_CHECK_EQUAL(0, coins->get<int>("Purple"));
    BOOST_CHECK_EQUAL(10, coins->get<int>("Yellow"));
}

BOOST_FIXTURE_TEST_CASE( CreateInvalidNode, StrategyTest ) 
{
    {
        boost::property_tree::ptree create;
        create.put("operation", "create");
        create.put("typename", "strategy_node");
        create.put("params.type", 110);

        expectError(mCore->executeCommandJson(writeJson(create)));
    }
    {
        boost::property_tree::ptree create;
        create.put("operation", "create");
        create.put("typename", "strategy_node");
        create.put("params.type", 0);
        create.put("params.parentNodeId", "wrong");

        expectError(mCore->executeCommandJson(writeJson(create)));
    }
    {
        boost::property_tree::ptree create;
        create.put("operation", "create");
        create.put("typename", "strategy_node");
        create.put("params.type", 0);
        create.put("defined_id", "g0");

        expectId(mCore->executeCommandJson(writeJson(create)));

        create.put("defined_id", "g1");
        create.put("params.parentNodeId", "g0");

        expectId(mCore->executeCommandJson(writeJson(create)));

        create.put("defined_id", "g2");
        create.put("params.parentNodeId", "g1");

        expectId(mCore->executeCommandJson(writeJson(create)));

        boost::property_tree::ptree modify;
        modify.put("operation", "modify");
        modify.put("params.parentNodeId", "g2");
        modify.put("id", "g0");

        expectError(mCore->executeCommandJson(writeJson(modify)));
    }
}

