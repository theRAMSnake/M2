#include <boost/test/unit_test.hpp>
#include <Core3/Core.hpp>
#include <Core3/Json.hpp>

class Core3Test
{
public:
   Core3Test()
   {
      
   }

protected:
   
};

BOOST_FIXTURE_TEST_CASE( Core3Test_1, Core3Test ) 
{  
   system("rm tmp.db");
   
   materia3::Core core({"tmp.db"});

   auto& session = core.createSession("test");

   materia3::Json j;
   j.set("type", "text");
   j.set("text", "test");

   session.forwardMessage(materia::Id("object_manager"), "create", j.str());

   auto responce = session.pick();

   BOOST_CHECK_EQUAL(responce.sender, materia::Id("object_manager"));
   BOOST_CHECK_EQUAL(responce.receiver,  materia::Id("test"));
   BOOST_REQUIRE_EQUAL(responce.type, "result");

   materia3::Json r(responce.content);
   BOOST_CHECK(r.contains("created_id"));

   session.forwardMessage(r.get<materia::Id>("created_id"), "describe", "");

   auto responce2 = session.pick();

   BOOST_CHECK_EQUAL(responce2.sender, r.get<materia::Id>("created_id"));
   BOOST_CHECK_EQUAL(responce2.receiver,  materia::Id("test"));
   BOOST_CHECK_EQUAL(responce2.type, "description");

   materia3::Json r2(responce2.content);
   BOOST_CHECK_EQUAL(r2.get<materia::Id>("id"), r.get<materia::Id>("created_id"));
   BOOST_CHECK_EQUAL("test", r2.get<std::string>("text"));
}

BOOST_FIXTURE_TEST_CASE( ListTest, Core3Test ) 
{  
   system("rm tmp.db");
   
   materia3::Core core({"tmp.db"});

   auto& session = core.createSession("test");

   materia3::Json j;
   j.set("type", "list");

   session.forwardMessage(materia::Id("object_manager"), "create", j.str());

   auto responce = session.pick();
   materia3::Json r(responce.content);
   BOOST_CHECK(r.contains("created_id"));
   auto listId = r.get<materia::Id>("created_id");

   std::vector<materia::Id> ids;
   for(int i = 0; i < 3; ++i)
   {
      materia3::Json cr;
      cr.set("type", "text");
      cr.set("text", "test");

      session.forwardMessage(materia::Id("object_manager"), "create", cr.str());

      auto responce2 = session.pick();
      materia3::Json r2(responce2.content);
      BOOST_CHECK(r2.contains("created_id"));
      ids.push_back(r2.get<materia::Id>("created_id"));

      materia3::Json insert;
      insert.set("id", r2.get<materia::Id>("created_id"));
      session.forwardMessage(listId, "insert", insert.str());
   }

   session.forwardMessage(listId, "describe", "");

   responce = session.pick();
   materia3::Json listItemsJson(responce.content);

   std::vector<materia::Id> listItems;
   listItemsJson.get("items", listItems);

   BOOST_CHECK(ids == listItems);

   for(int i = 0; i < 2; ++i)
   {
      materia3::Json remove;
      remove.set("id", listItems[i]);
      session.forwardMessage(listId, "erase", remove.str());

      std::this_thread::sleep_for(std::chrono::milliseconds(100));

      //Check that item is no longer there
      session.forwardMessage(listItems[i], "describe", "");
      BOOST_CHECK(session.noResponce());
   }

   session.forwardMessage(listId, "destroy", "");

   std::this_thread::sleep_for(std::chrono::milliseconds(100));
   
   session.forwardMessage(listItems[2], "describe", "");
   BOOST_CHECK(session.noResponce());
}