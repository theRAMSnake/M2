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

BOOST_FIXTURE_TEST_CASE( TestValueExp, ExpressionsTest ) 
{  
   materia::Params p;
   p.put("some", 6);

   BOOST_CHECK_EQUAL(5, std::get<int>(materia::parseExpression("5")->evaluate(p)));
   BOOST_CHECK_EQUAL(5.0, std::get<double>(materia::parseExpression("5.0")->evaluate(p)));
   BOOST_CHECK_EQUAL("5", std::get<std::string>(materia::parseExpression("\"5\"")->evaluate(p)));
   BOOST_CHECK_EQUAL(true, std::get<bool>(materia::parseExpression("true")->evaluate(p)));
   BOOST_CHECK_EQUAL(6, std::get<int>(materia::parseExpression(".some")->evaluate(p)));
}

BOOST_FIXTURE_TEST_CASE( TestSimpleBinaryExp, ExpressionsTest ) 
{
   materia::Params p;
   p.put("some", 6);

   BOOST_CHECK_EQUAL(true, std::get<bool>(materia::parseExpression(".some < 10")->evaluate(p)));
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::parseExpression(".some < 5")->evaluate(p)));

   BOOST_CHECK_EQUAL(true, std::get<bool>(materia::parseExpression(".some > 5")->evaluate(p)));
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::parseExpression(".some > 10")->evaluate(p)));

   BOOST_CHECK_EQUAL(true, std::get<bool>(materia::parseExpression(".some = 6")->evaluate(p)));
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::parseExpression("7 = .some")->evaluate(p)));
}

BOOST_FIXTURE_TEST_CASE( TestContainsExp, ExpressionsTest ) 
{
   materia::Params p;
   p.put("some", "aaa");
   p.put("some1", "aa");
   p.put("some2", "bb");

   BOOST_CHECK_EQUAL(true, std::get<bool>(materia::parseExpression(".some contains .some1")->evaluate(p)));
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::parseExpression(".some contains .some2")->evaluate(p)));
}

BOOST_FIXTURE_TEST_CASE( TestAndOrExp, ExpressionsTest ) 
{
   materia::Params p;
   p.put("some", "aaa");
   p.put("some1", "aa");
   p.put("some3", "bb");
   p.put("some2", 6);

   BOOST_CHECK_EQUAL(true, std::get<bool>(materia::parseExpression(".some contains .some1 AND .some2 < 10")->evaluate(p)));
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::parseExpression(".some contains .some1 AND .some2 < 1")->evaluate(p)));

   BOOST_CHECK_EQUAL(true, std::get<bool>(materia::parseExpression(".some contains .some1 OR .some2 < 10")->evaluate(p)));
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::parseExpression(".some contains .some3 OR .some2 < 1")->evaluate(p)));
}