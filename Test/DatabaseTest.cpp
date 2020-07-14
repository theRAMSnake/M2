#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "../Core/private/Database.hpp"

class DatabaseTest
{
public:
   DatabaseTest()
   {
      system("rm Test.db");
      mDb.reset(new materia::Database("Test.db"));
   }

protected:
   std::unique_ptr<materia::Database> mDb;
};

BOOST_FIXTURE_TEST_CASE( Test1, DatabaseTest ) 
{ 
   std::vector<std::pair<materia::Id, std::string>> TEST_DATA = {
      {materia::Id("1"), "1"},
      {materia::Id("2"), "2"},
      {materia::Id("3"), "3"},
      {materia::Id("4"), "4"},
      {materia::Id("5"), "5"}
   };

   auto table = mDb->getTable("test");
   for(auto x : TEST_DATA)
   {
      table->store(x.first, x.second);
   }

   std::vector<std::pair<materia::Id, std::string>> result;
   std::function<void(std::string id, std::string json)> f = [&](std::string id, std::string json)->void {
      result.push_back({materia::Id(id), json});
   };
   std::this_thread::sleep_for(std::chrono::seconds(1));
   table->foreach(f);
   
   BOOST_CHECK_EQUAL(5, result.size());

   for(std::size_t i = 0; i < result.size(); ++i)
   {
      BOOST_CHECK(TEST_DATA[i] == result[i]);
   }

   table->erase(materia::Id("3"));
   TEST_DATA.erase(TEST_DATA.begin() + 2);
   result.clear();

   std::this_thread::sleep_for(std::chrono::seconds(1));
   table->foreach(f);

   BOOST_CHECK_EQUAL(4, result.size());
   for(std::size_t i = 0; i < result.size(); ++i)
   {
      BOOST_CHECK(TEST_DATA[i] == result[i]);
   }
}