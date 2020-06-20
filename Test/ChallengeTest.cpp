#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "../Core/private/JsonSerializer.hpp"
#include <Core/ICore3.hpp>

class ChallengeTest
{
public:
   ChallengeTest()
   {
      system("rm Test.db");
      mCore = materia::createCore({"Test.db"});
   }

protected:

   std::shared_ptr<materia::ICore3> mCore;
};

BOOST_FIXTURE_TEST_CASE( AddPoints_ch, ChallengeTest ) 
{
    boost::property_tree::ptree create;
    create.put("operation", "create");
    create.put("typename", "challenge_item");
    create.put("params.points", 0);
    create.put("params.pointsNeeded", 10);
    create.put("params.advance", 5);

    auto r = readJson<boost::property_tree::ptree>(mCore->executeCommandJson(writeJson(create)));

    {
      boost::property_tree::ptree modify;
      modify.put("operation", "modify");
      modify.put("params.points", 7);
      modify.put("params.pointsNeeded", 10);
      modify.put("params.advance", 5);
      modify.put("id", r.get<std::string>("result_id"));

      mCore->executeCommandJson(writeJson(modify));

      boost::property_tree::ptree query;
      query.put("operation", "query");
      query.put("filter", "IS(challenge_item)");

      auto result = mCore->executeCommandJson(writeJson(query));

      auto ol = readJson<boost::property_tree::ptree>(result);

      for(auto& v : ol.get_child("object_list"))
      {
         BOOST_CHECK_EQUAL(7, v.second.get<int>("points"));
         BOOST_CHECK_EQUAL(10, v.second.get<int>("pointsNeeded"));
         BOOST_CHECK_EQUAL(0, v.second.get<int>("level"));
      }
    }

    {
      boost::property_tree::ptree modify;
      modify.put("operation", "modify");
      modify.put("params.points", 11);
      modify.put("params.pointsNeeded", 10);
      modify.put("params.advance", 5);
      modify.put("id", r.get<std::string>("result_id"));

      mCore->executeCommandJson(writeJson(modify));

      boost::property_tree::ptree query;
      query.put("operation", "query");
      query.put("filter", "IS(challenge_item)");

      auto result = mCore->executeCommandJson(writeJson(query));

      auto ol = readJson<boost::property_tree::ptree>(result);

      for(auto& v : ol.get_child("object_list"))
      {
         BOOST_CHECK_EQUAL(1, v.second.get<int>("points"));
         BOOST_CHECK_EQUAL(15, v.second.get<int>("pointsNeeded"));
         BOOST_CHECK_EQUAL(1, v.second.get<int>("level"));
      }
    }
}

BOOST_FIXTURE_TEST_CASE( WeeklyTest1, ChallengeTest ) 
{
   {
      boost::property_tree::ptree create;
      create.put("operation", "create");
      create.put("typename", "challenge_item");
      create.put("params.points", 3);
      create.put("params.pointsNeeded", 10);
      create.put("params.advance", 5);
      create.put("params.resetWeekly", false);

      auto r = readJson<boost::property_tree::ptree>(mCore->executeCommandJson(writeJson(create)));

      mCore->onNewWeek();

      boost::property_tree::ptree query;
      query.put("operation", "query");
      query.put("filter", "IS(challenge_item)");

      auto result = mCore->executeCommandJson(writeJson(query));

      auto ol = readJson<boost::property_tree::ptree>(result);

      for(auto& v : ol.get_child("object_list"))
      {
         BOOST_CHECK_EQUAL(3, v.second.get<int>("points"));
      }
   } 
}

BOOST_FIXTURE_TEST_CASE( WeeklyTest2, ChallengeTest ) 
{
   {
      boost::property_tree::ptree create;
      create.put("operation", "create");
      create.put("typename", "challenge_item");
      create.put("params.points", 3);
      create.put("params.pointsNeeded", 10);
      create.put("params.advance", 5);
      create.put("params.resetWeekly", true);

      auto r = readJson<boost::property_tree::ptree>(mCore->executeCommandJson(writeJson(create)));

      mCore->onNewWeek();

      boost::property_tree::ptree query;
      query.put("operation", "query");
      query.put("filter", "IS(challenge_item)");

      auto result = mCore->executeCommandJson(writeJson(query));

      auto ol = readJson<boost::property_tree::ptree>(result);

      for(auto& v : ol.get_child("object_list"))
      {
         BOOST_CHECK_EQUAL(0, v.second.get<int>("points"));
      }
   } 
}