#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <Core/ICore3.hpp>
#include <chrono>
#include "../Core/private/JsonSerializer.hpp"
#include "Utils.hpp"

class ChestAndItemsTest
{
public:
   ChestAndItemsTest()
   {
      system("rm Test.db");
      mCore = materia::createCore({"Test.db"});

      {
         boost::property_tree::ptree create;
         create.put("operation", "create");
         create.put("typename", "object");
         create.put("defined_id", "config.reward");
         create.put("params.chests.id", "chests");
         create.put("params.chestValues", 25);
         create.put("params.chests.typename", "object");
         create.put("params.chests.0.id", "id0");
         create.put("params.chests.0.typename", "object");
         create.put("params.chests.0.name", "empty");
         create.put("params.chests.0.weight", 0.5);
         create.put("params.chests.1.id", "id1");
         create.put("params.chests.1.typename", "object");
         create.put("params.chests.1.name", "premium");
         create.put("params.chests.1.weight", 0.5);
         create.put("params.chests.1.premium", "typea");
         create.put("params.chests.2.id", "id2");
         create.put("params.chests.2.typename", "object");
         create.put("params.chests.2.name", "fillwith");
         create.put("params.chests.2.weight", 0.5);
         create.put("params.chests.2.fillwith", "typea;typeb");
         create.put("params.chests.3.id", "id3");
         create.put("params.chests.3.typename", "object");
         create.put("params.chests.3.name", "fillwith_premium");
         create.put("params.chests.3.weight", 0.5);
         create.put("params.chests.3.fillwith", "typea");
         create.put("params.chests.3.premium", "typeb");
         create.put("params.items.id", "items");
         create.put("params.items.typename", "object");
         create.put("params.items.0.id", "id0");
         create.put("params.items.0.typename", "object");
         create.put("params.items.0.name", "a1");
         create.put("params.items.0.value", 1);
         create.put("params.items.0.category", "typea");
         create.put("params.items.1.id", "id1");
         create.put("params.items.1.typename", "object");
         create.put("params.items.1.name", "a2");
         create.put("params.items.1.value", 3);
         create.put("params.items.1.category", "typea");
         create.put("params.items.2.id", "id0");
         create.put("params.items.2.typename", "object");
         create.put("params.items.2.name", "b1");
         create.put("params.items.2.value", 1);
         create.put("params.items.2.category", "typeb");
         create.put("params.items.3.id", "id1");
         create.put("params.items.3.typename", "object");
         create.put("params.items.3.name", "b2");
         create.put("params.items.3.value", 4);
         create.put("params.items.3.category", "typeb");
         create.put("params.chests.4.id", "id4");
         create.put("params.chests.4.typename", "object");
         create.put("params.chests.4.name", "moded");
         create.put("params.chests.4.weight", 0.5);
         create.put("params.chests.4.premium", "mods");
         create.put("params.items.4.id", "id4");
         create.put("params.items.4.typename", "object");
         create.put("params.items.4.name", "mod");
         create.put("params.items.4.value", 4);
         create.put("params.items.4.category", "mods");
         create.put("params.items.4.behavior.id", "bhv");
         create.put("params.items.4.behavior.typename", "object");
         create.put("params.items.4.behavior.type", "add_mod");
         create.put("params.items.4.behavior.name", "mod");
         create.put("params.items.4.behavior.duration", 7);
         create.put("params.items.4.behavior.value", 1);
         create.put("params.chests.5.id", "id5");
         create.put("params.chests.5.typename", "object");
         create.put("params.chests.5.name", "token");
         create.put("params.chests.5.weight", 0.5);
         create.put("params.chests.5.fillwith", "token");
         create.put("params.items.5.id", "id5");
         create.put("params.items.5.typename", "object");
         create.put("params.items.5.name", "A Token");
         create.put("params.items.5.value", 1);
         create.put("params.items.5.amount", 2);
         create.put("params.items.5.category", "token");
         create.put("params.chests.6.id", "id6");
         create.put("params.chests.6.typename", "object");
         create.put("params.chests.6.name", "from_list");
         create.put("params.chests.6.weight", 0.5);
         create.put("params.chests.6.premium", "from_list");
         create.put("params.items.6.id", "id6");
         create.put("params.items.6.typename", "object");
         create.put("params.items.6.name", "fl");
         create.put("params.items.6.value", 4);
         create.put("params.items.6.category", "from_list");
         create.put("params.items.6.behavior.id", "bhv");
         create.put("params.items.6.behavior.typename", "object");
         create.put("params.items.6.behavior.type", "fetch_from_list");
         create.put("params.items.6.behavior.source", "list");

         expectId(mCore->executeCommandJson(writeJson(create)));

         boost::property_tree::ptree createList;
         createList.put("operation", "create");
         createList.put("typename", "simple_list");
         createList.put("defined_id", "list");
         createList.put("params", "");
         expectId(mCore->executeCommandJson(writeJson(createList)));

         boost::property_tree::ptree push;
         push.put("operation", "push");
         push.put("listId", "list");
         push.put("value", "v1");
         mCore->executeCommandJson(writeJson(push));
         
         push.put("value", "v2");
         mCore->executeCommandJson(writeJson(push));
      }
   }

protected:
   void grantChest()
   {
      boost::property_tree::ptree rwd;
      rwd.put("operation", "reward");
      rwd.put("points", 25);
      mCore->executeCommandJson(writeJson(rwd));
   }

   std::optional<boost::property_tree::ptree> openChest()
   {
      boost::property_tree::ptree rwd;
      rwd.put("operation", "useChest");
      auto result = mCore->executeCommandJson(writeJson(rwd));

      try
      {
          auto ol = readJson<boost::property_tree::ptree>(result);
            
          for(auto& v : ol.get_child("object_list"))
          {
             return v.second;
          }
      }
      catch(...)
      {
          std::cout << "Unable to parse result: " << result << std::endl;
      }
      return std::optional<boost::property_tree::ptree>();
   }

   std::shared_ptr<materia::ICore3> mCore;
};

BOOST_FIXTURE_TEST_CASE( TestGrantChestSimple, ChestAndItemsTest ) 
{
   grantChest(); 
}

BOOST_FIXTURE_TEST_CASE( TestOpenChestNoCrash, ChestAndItemsTest ) 
{
   grantChest(); 
   auto result = openChest();

   BOOST_CHECK(result);
}

BOOST_FIXTURE_TEST_CASE( TestGen100Chests, ChestAndItemsTest ) 
{
   for(int i = 0; i < 100; ++i)
   {
       grantChest(); 
       auto result = openChest();

       BOOST_CHECK(result);
       
       auto chestType = result->get<std::string>("chestType");
       if(chestType == "empty")
       {
           //Expect no items added
           BOOST_CHECK_EQUAL(0, count(queryAll("reward_item", *mCore)));
       }
       else if(chestType == "premium")
       {
           //Expect exactly one item of typea
           BOOST_CHECK_EQUAL(1, count(queryAll("reward_item", *mCore)));
           auto item = queryFirst("reward_item", *mCore);
           BOOST_CHECK_EQUAL('a', item.get<std::string>("name")[0]);
       }
       else if(chestType == "fillwith")
       {
           //Expect many items
           BOOST_CHECK(5 < count(queryAll("reward_item", *mCore)));
       }
       else if(chestType == "fillwith_premium")
       {
           //Expect one b item and rest a items
           BOOST_CHECK_EQUAL(1, count(queryCondition("reward_item", ".name = \"b1\" OR .name = \"b2\"", *mCore)));
           BOOST_CHECK(1 < count(queryCondition("reward_item", ".name = \"a1\" OR .name = \"a2\"", *mCore)));
       }
       else if(chestType == "moded")
       {
           //Expect to see a mod
           auto mod = queryFirst("reward_modifier", *mCore);
           BOOST_CHECK_EQUAL("mod", mod.get<std::string>("desc"));

           auto atLeastTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now() + std::chrono::days(7));

           BOOST_CHECK(atLeastTime <= mod.get<int>("validUntil"));
           BOOST_CHECK_EQUAL(1, mod.get<int>("value"));
       }
       else if(chestType == "token")
       {
           //Expect exactly one stacked item
           BOOST_CHECK_EQUAL(1, count(queryAll("reward_item", *mCore)));
           auto item = queryFirst("reward_item", *mCore);

           BOOST_CHECK_EQUAL("A Token", item.get<std::string>("name"));
           BOOST_CHECK_EQUAL(1+24*2, item.get<int>("amount"));
       }
       else if(chestType == "from_list")
       {
           //Expect exactly one item from the list
           BOOST_CHECK_EQUAL(1, count(queryAll("reward_item", *mCore)));
           auto item = queryFirst("reward_item", *mCore);

           BOOST_CHECK_EQUAL('v', item.get<std::string>("name")[0]);
       }
       else
       {
           //Unknown chest type
           BOOST_CHECK(false);
       }
       deleteAll("reward_item", *mCore); 
       deleteAll("reward_modifier", *mCore); 
   }
}
