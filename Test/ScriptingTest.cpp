#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "../Core/private/ScriptRunner.hpp"

class ScriptingTest
{
public:
   ScriptingTest()
   {
      
   }

protected:
};

BOOST_FIXTURE_TEST_CASE( ScriptingTest_HW, ScriptingTest ) 
{
   materia::ScriptRunner runner;

   BOOST_CHECK(runner.exec("a = 5"));
   BOOST_CHECK(!runner.exec("b = a"));
}