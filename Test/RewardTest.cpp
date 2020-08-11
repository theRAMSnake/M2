#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <Core/ICore3.hpp>
#include "../Core/private/JsonSerializer.hpp"

class RewardTest
{
public:
   RewardTest()
   {
      system("rm Test.db");
      mCore = materia::createCore({"Test.db"});
   }

protected:

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