// >, <, =, contains, and, or
// 5; name;
// 6 > money;
// 7 < money AND name contains 'dd'
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "../Core/private/Expressions.hpp"

class ExpressionsTest
{
public:
   ExpressionsTest()
   {
      
   }

protected:

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

   BOOST_CHECK_EQUAL(5, std::get<std::int64_t>(materia::parseExpression("5")->evaluate(p)));
   BOOST_CHECK_EQUAL(5.0, std::get<double>(materia::parseExpression("5.0")->evaluate(p)));
   BOOST_CHECK_EQUAL("5", std::get<std::string>(materia::parseExpression("\"5\"")->evaluate(p)));
   BOOST_CHECK_EQUAL(true, std::get<bool>(materia::parseExpression("true")->evaluate(p)));
   BOOST_CHECK_EQUAL(6, std::get<std::int64_t>(materia::parseExpression(".some")->evaluate(p)));
}

BOOST_FIXTURE_TEST_CASE( TestSimpleBinaryExp, ExpressionsTest ) 
{
   materia::Object p(gType, materia::Id::Invalid);
   p["some"] = 6;

   BOOST_CHECK_EQUAL(true, std::get<bool>(materia::parseExpression(".some < 10")->evaluate(p)));
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::parseExpression(".some < 5")->evaluate(p)));

   BOOST_CHECK_EQUAL(true, std::get<bool>(materia::parseExpression(".some > 5")->evaluate(p)));
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::parseExpression(".some > 10")->evaluate(p)));

   BOOST_CHECK_EQUAL(true, std::get<bool>(materia::parseExpression(".some = 6")->evaluate(p)));
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::parseExpression("7 = .some")->evaluate(p)));
}

BOOST_FIXTURE_TEST_CASE( TestContainsExp, ExpressionsTest ) 
{
   materia::Object p(gType, materia::Id::Invalid);
   p["som"] = std::string("aaa");
   p["some1"] = std::string("aa");
   p["some2"] = std::string("bb");

   std::cout << p.toJson();
   BOOST_CHECK_EQUAL(true, std::get<bool>(materia::parseExpression(".som contains .some1")->evaluate(p)));
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::parseExpression(".som contains .some2")->evaluate(p)));
}

BOOST_FIXTURE_TEST_CASE( TestAndOrExp, ExpressionsTest ) 
{
   materia::Object p(gType, materia::Id::Invalid);
   p["some"] = 6;
   p["som"] = std::string("aaa");
   p["some1"] = std::string("aa");
   p["some3"] = std::string("bb");

   BOOST_CHECK_EQUAL(true, std::get<bool>(materia::parseExpression(".som contains .some1 AND .some < 10")->evaluate(p)));
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::parseExpression(".som contains .some1 AND .some < 1")->evaluate(p)));

   BOOST_CHECK_EQUAL(true, std::get<bool>(materia::parseExpression(".som contains .some1 OR .some < 10")->evaluate(p)));
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::parseExpression(".som contains .some3 OR .some < 1")->evaluate(p)));
}