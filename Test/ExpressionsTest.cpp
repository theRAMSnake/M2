// >, <, =, contains, and, or
// 5; name;
// 6 > money;
// 7 < money AND name contains 'dd'
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "../Core/private/Expressions.hpp"
#include "../Core/private/Database.hpp"
#include "../Core/private/Connections.hpp"

class ExpressionsTest
{
public:
   ExpressionsTest()
   {
      system("rm Test.db");
      mDb.reset(new materia::Database("Test.db"));
      mConnections = std::make_unique<materia::Connections>(*mDb);
   }

protected:
   std::unique_ptr<materia::Database> mDb;
   std::unique_ptr<materia::Connections> mConnections;
};

materia::TypeDef gType {
   "test",
   "test",
   {{"some", materia::Type::Int}}
};

BOOST_FIXTURE_TEST_CASE( TestValueExp, ExpressionsTest ) 
{  
   materia::Object p(gType, materia::Id::Invalid);
   p["some"] = 6;

   BOOST_CHECK_EQUAL(5, std::get<std::int64_t>(materia::parseExpression("5")->evaluate(p, *mConnections)));
   BOOST_CHECK_EQUAL(5.0, std::get<double>(materia::parseExpression("5.0")->evaluate(p, *mConnections)));
   BOOST_CHECK_EQUAL("5", std::get<std::string>(materia::parseExpression("\"5\"")->evaluate(p, *mConnections)));
   BOOST_CHECK_EQUAL(true, std::get<bool>(materia::parseExpression("true")->evaluate(p, *mConnections)));
   BOOST_CHECK_EQUAL(6, std::get<std::int64_t>(materia::parseExpression(".some")->evaluate(p, *mConnections)));
}

BOOST_FIXTURE_TEST_CASE( TestSimpleBinaryExp, ExpressionsTest ) 
{
   materia::Object p(gType, materia::Id::Invalid);
   p["some"] = 6;

   BOOST_CHECK_EQUAL(true, std::get<bool>(materia::parseExpression(".some < 10")->evaluate(p, *mConnections)));
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::parseExpression(".some < 5")->evaluate(p, *mConnections)));

   BOOST_CHECK_EQUAL(true, std::get<bool>(materia::parseExpression(".some > 5")->evaluate(p, *mConnections)));
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::parseExpression(".some > 10")->evaluate(p, *mConnections)));

   BOOST_CHECK_EQUAL(true, std::get<bool>(materia::parseExpression(".some = 6")->evaluate(p, *mConnections)));
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::parseExpression("7 = .some")->evaluate(p, *mConnections)));
}

BOOST_FIXTURE_TEST_CASE( TestContainsExp, ExpressionsTest ) 
{
   materia::Object p(gType, materia::Id::Invalid);
   p["som"] = std::string("aaa");
   p["some1"] = std::string("aa");
   p["some2"] = std::string("bb");

   BOOST_CHECK_EQUAL(true, std::get<bool>(materia::parseExpression(".som contains .some1")->evaluate(p, *mConnections)));
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::parseExpression(".som contains .some2")->evaluate(p, *mConnections)));
}

BOOST_FIXTURE_TEST_CASE( TestAndOrExp, ExpressionsTest ) 
{
   materia::Object p(gType, materia::Id::Invalid);
   p["some"] = 6;
   p["som"] = std::string("aaa");
   p["some1"] = std::string("aa");
   p["some3"] = std::string("bb");

   BOOST_CHECK_EQUAL(true, std::get<bool>(materia::parseExpression(".som contains .some1 AND .some < 10")->evaluate(p, *mConnections)));
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::parseExpression(".som contains .some1 AND .some < 1")->evaluate(p, *mConnections)));

   BOOST_CHECK_EQUAL(true, std::get<bool>(materia::parseExpression(".som contains .some1 OR .some < 10")->evaluate(p, *mConnections)));
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::parseExpression(".som contains .some3 OR .some < 1")->evaluate(p, *mConnections)));
}

BOOST_FIXTURE_TEST_CASE( TestContainsString, ExpressionsTest ) 
{
   materia::Object p(gType, materia::Id::Invalid);
   p["som"] = std::string("aaa");
   p["p"] = 5;

   BOOST_CHECK_EQUAL(true, std::get<bool>(materia::parseExpression(".som contains \"aa\"")->evaluate(p, *mConnections)));
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::parseExpression(".p = 5 AND .som contains \"bb\"")->evaluate(p, *mConnections)));
}

BOOST_FIXTURE_TEST_CASE( TestCons, ExpressionsTest ) 
{
   materia::Object p(gType, materia::Id("id1"));
   materia::Object p2(gType, materia::Id("id2"));
   materia::Object p3(gType, materia::Id("id3"));
   
   mConnections->create(materia::Id("id1"), materia::Id("id2"), materia::ConnectionType::Hierarchy);
   mConnections->create(materia::Id("id1"), materia::Id("id2"), materia::ConnectionType::Reference);
   mConnections->create(materia::Id("id1"), materia::Id("id2"), materia::ConnectionType::Extension);
   mConnections->create(materia::Id("id1"), materia::Id("id2"), materia::ConnectionType::Requirement);

   BOOST_CHECK_EQUAL(true, std::get<bool>(materia::parseExpression("ParentOf(id2)")->evaluate(p, *mConnections)));
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::parseExpression("ParentOf(id1)")->evaluate(p2, *mConnections)));
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::parseExpression("ChildOf(id2)")->evaluate(p, *mConnections)));
   BOOST_CHECK_EQUAL(true, std::get<bool>(materia::parseExpression("ChildOf(id1)")->evaluate(p2, *mConnections)));

   BOOST_CHECK_EQUAL(true, std::get<bool>(materia::parseExpression("Refers(id2)")->evaluate(p, *mConnections)));
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::parseExpression("Refers(id1)")->evaluate(p2, *mConnections)));
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::parseExpression("ReferedBy(id2)")->evaluate(p, *mConnections)));
   BOOST_CHECK_EQUAL(true, std::get<bool>(materia::parseExpression("ReferedBy(id1)")->evaluate(p2, *mConnections)));

   BOOST_CHECK_EQUAL(true, std::get<bool>(materia::parseExpression("ExtendedBy(id2)")->evaluate(p, *mConnections)));
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::parseExpression("ExtendedBy(id1)")->evaluate(p2, *mConnections)));
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::parseExpression("Extends(id2)")->evaluate(p, *mConnections)));
   BOOST_CHECK_EQUAL(true, std::get<bool>(materia::parseExpression("Extends(id1)")->evaluate(p2, *mConnections)));

   BOOST_CHECK_EQUAL(true, std::get<bool>(materia::parseExpression("Enables(id2)")->evaluate(p, *mConnections)));
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::parseExpression("Enables(id1)")->evaluate(p2, *mConnections)));
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::parseExpression("Requires(id2)")->evaluate(p, *mConnections)));
   BOOST_CHECK_EQUAL(true, std::get<bool>(materia::parseExpression("Requires(id1)")->evaluate(p2, *mConnections)));

   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::parseExpression("ParentOf(id1)")->evaluate(p3, *mConnections)));
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::parseExpression("ChildOf(id1)")->evaluate(p3, *mConnections)));

   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::parseExpression("Refers(id1)")->evaluate(p3, *mConnections)));
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::parseExpression("ReferedBy(id1)")->evaluate(p3, *mConnections)));

   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::parseExpression("ExtendedBy(id1)")->evaluate(p3, *mConnections)));
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::parseExpression("Extends(id1)")->evaluate(p3, *mConnections)));

   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::parseExpression("Enables(id1)")->evaluate(p3, *mConnections)));
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::parseExpression("Requires(id1)")->evaluate(p3, *mConnections)));
}

