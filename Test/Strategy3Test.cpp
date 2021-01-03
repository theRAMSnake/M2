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

BOOST_FIXTURE_TEST_CASE( UpdateWaitNode, StrategyTest ) 
{
    boost::property_tree::ptree create;
    create.put("operation", "create");
    create.put("typename", "strategy_node");
    create.put("defined_id", "w1");
    create.put("params.type", 4);
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
        create.put("params.parentNodeId", "subject");
        expectId(mCore->executeCommandJson(writeJson(create)));
    }

    boost::property_tree::ptree createLink;
    createLink.put("operation", "create");
    createLink.put("typename", "strategy_link");
    createLink.put("params.idFrom", "n1");
    createLink.put("params.idTo", "n2");
    expectId(mCore->executeCommandJson(writeJson(createLink)));

    createLink.put("params.idFrom", "n1");
    createLink.put("params.idTo", "subject");
    expectId(mCore->executeCommandJson(writeJson(createLink)));

    createLink.put("params.idFrom", "subject");
    createLink.put("params.idTo", "n2");
    expectId(mCore->executeCommandJson(writeJson(createLink)));

    boost::property_tree::ptree destroy;
    destroy.put("operation", "destroy");
    destroy.put("id", "subject");

    mCore->executeCommandJson(writeJson(destroy));

    BOOST_CHECK_EQUAL(2, count(queryAll("strategy_node", *mCore)));
    BOOST_CHECK_EQUAL(1, count(queryAll("strategy_link", *mCore)));

    BOOST_CHECK(query("n1", *mCore));
    BOOST_CHECK(query("n2", *mCore));

    auto l = queryFirst("strategy_link", *mCore);
    BOOST_CHECK_EQUAL("n1", l.get<std::string>("idFrom"));
    BOOST_CHECK_EQUAL("n2", l.get<std::string>("idTo"));
}

BOOST_FIXTURE_TEST_CASE( CounterIsAchievedCalculation, StrategyTest ) 
{
    boost::property_tree::ptree create;
    create.put("operation", "create");
    create.put("typename", "strategy_node");
    create.put("defined_id", "counter");
    create.put("params.type", 2);
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

BOOST_FIXTURE_TEST_CASE( Rewarding, StrategyTest ) 
{
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

    auto p = queryFirst("reward_pool", *mCore);
    BOOST_CHECK_EQUAL(10, p.get<int>("amount"));
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

BOOST_FIXTURE_TEST_CASE( CreateInvalidLink, StrategyTest ) 
{
    boost::property_tree::ptree create;
    create.put("operation", "create");
    create.put("typename", "strategy_node");
    create.put("params.type", 4);
    create.put("defined_id", "n1");
    expectId(mCore->executeCommandJson(writeJson(create)));

    create.put("defined_id", "n2");
    expectId(mCore->executeCommandJson(writeJson(create)));

    create.put("defined_id", "n3");
    expectId(mCore->executeCommandJson(writeJson(create)));

    {
        boost::property_tree::ptree createLink;
        createLink.put("operation", "create");
        createLink.put("typename", "strategy_link");
        createLink.put("params.idFrom", "n1");
        createLink.put("params.idTo", "n2");

        expectId(mCore->executeCommandJson(writeJson(createLink)));

        expectError(mCore->executeCommandJson(writeJson(createLink)));

        createLink.put("params.idTo", "n1");

        expectError(mCore->executeCommandJson(writeJson(createLink)));
    }
    {
        boost::property_tree::ptree createLink;
        createLink.put("operation", "create");
        createLink.put("typename", "strategy_link");
        createLink.put("params.idFrom", "fsdfsd");
        createLink.put("params.idTo", "sdfsdf");

        expectError(mCore->executeCommandJson(writeJson(createLink)));
    }
    {
        boost::property_tree::ptree createLink;
        createLink.put("operation", "create");
        createLink.put("typename", "strategy_link");
        createLink.put("params.idFrom", "n2");
        createLink.put("params.idTo", "n3");

        expectId(mCore->executeCommandJson(writeJson(createLink)));

        createLink.put("params.idFrom", "n3");
        createLink.put("params.idTo", "n1");

        expectError(mCore->executeCommandJson(writeJson(createLink)));
    }
}