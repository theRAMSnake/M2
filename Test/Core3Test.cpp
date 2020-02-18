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
   BOOST_CHECK_EQUAL(responce.type, "result");

   materia3::Json r(responce.content);
   BOOST_CHECK(r.contains("id"));

   session.forwardMessage(r.get<materia::Id>("id"), "describe", "");

   auto responce2 = session.pick();

   BOOST_CHECK_EQUAL(responce2.sender, r.get<materia::Id>("id"));
   BOOST_CHECK_EQUAL(responce2.receiver,  materia::Id("test"));
   BOOST_CHECK_EQUAL(responce2.type, "description");

   materia3::Json r2(responce2.content);
   BOOST_CHECK_EQUAL(r2.get<materia::Id>("id"), r.get<materia::Id>("id"));
   BOOST_CHECK_EQUAL("test", r.get<std::string>("text"));
}
