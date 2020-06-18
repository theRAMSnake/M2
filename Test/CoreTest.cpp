#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <Core/ICore3.hpp>

static std::shared_ptr<materia::ICore> createTestCore()
{
   system("rm Test.db");
   return materia::createCore({"Test.db"});
}

class CoreTest
{
public:
   CoreTest()
   : mCore(createTestCore())
   {
      
   }

protected:
   std::shared_ptr<materia::ICore> mCore;
};

BOOST_FIXTURE_TEST_CASE( NewDayTest, CoreTest ) 
{  
   mCore->onNewDay();
}