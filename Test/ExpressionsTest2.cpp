#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "../Core/private/Expressions.hpp"
#include "../Core/private/Database.hpp"
#include "../Core/private/Connections.hpp"

class Expressions2Test
{
public:
   Expressions2Test()
   {
      system("rm Test.db");
      mDb.reset(new materia::Database("Test.db"));
      mConnections = std::make_unique<materia::Connections>(*mDb);
   }

protected:

   template<class T>
   T eval(const std::string& src)
   {
       auto expr = materia::v2::parseExpression(src);
       if(!expr)
       {
           BOOST_REQUIRE_MESSAGE(expr, "Unable to parse: " + src);
       }

       return std::get<T>(expr->evaluate(mCtx));
   }

   std::unique_ptr<materia::Database> mDb;
   std::unique_ptr<materia::Connections> mConnections;
   materia::v2::InterpreterContext mCtx;
};


BOOST_FIXTURE_TEST_CASE( TestBoolean, Expressions2Test )
{
   BOOST_CHECK_EQUAL(true, eval<bool>("true"));
   BOOST_CHECK_EQUAL(false, eval<bool>("false"));
}

BOOST_FIXTURE_TEST_CASE( TestNot, Expressions2Test )
{
   BOOST_CHECK_EQUAL(true, eval<bool>("!false"));
   BOOST_CHECK_EQUAL(false, eval<bool>("!true"));
   BOOST_CHECK_EQUAL(false, eval<bool>("!!false"));
   BOOST_CHECK_EQUAL(true, eval<bool>("!!true"));
}

BOOST_FIXTURE_TEST_CASE( TestEq, Expressions2Test )
{
   BOOST_CHECK_EQUAL(true, eval<bool>("true = true"));
   BOOST_CHECK_EQUAL(false, eval<bool>("true = false"));
   BOOST_CHECK_EQUAL(true, eval<bool>("false = false"));
   BOOST_CHECK_EQUAL(false, eval<bool>("!true = true"));
   BOOST_CHECK_EQUAL(false, eval<bool>("true = !true"));
}
