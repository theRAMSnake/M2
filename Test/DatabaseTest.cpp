#include <boost/test/unit_test.hpp>
#include <Core3/Database.hpp>

class DatabaseTest
{
public:
   DatabaseTest()
   {
      
   }

protected:
   
};


BOOST_FIXTURE_TEST_CASE( DatabaseTest_1, DatabaseTest ) 
{  
    system("rm tmp.db");
    {
        materia3::Database db("tmp.db");
        auto slot1 = db.allocate(materia::Id("slot1"), "");
        auto slot2 = db.allocate(materia::Id("slot2"), "");

        slot1->put("info1");
        slot2->put("info2");
    }
    {
        materia3::Database db("tmp.db");
        auto slot1 = db.load(materia::Id("slot1"));
        auto slot2 = db.load(materia::Id("slot2"));
        auto slot3 = db.load(materia::Id("slot3"));

        BOOST_CHECK_EQUAL("info1", slot1->get());
        BOOST_CHECK_EQUAL("info2", slot2->get());
        BOOST_CHECK(slot3 == nullptr);
    }
    {
        materia3::Database db("tmp.db");
        auto slot1 = db.load(materia::Id("slot1"));
        auto slot2 = db.load(materia::Id("slot2"));

        slot1->clear();
        slot2->clear();
    }
    {
        materia3::Database db("tmp.db");
        auto slot1 = db.load(materia::Id("slot1"));
        auto slot2 = db.load(materia::Id("slot2"));
        auto slot3 = db.load(materia::Id("slot3"));

        BOOST_CHECK(slot1 == nullptr);
        BOOST_CHECK(slot2 == nullptr);
        BOOST_CHECK(slot3 == nullptr);
    }
}
