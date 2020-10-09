#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <Core/ICore3.hpp>

static std::shared_ptr<materia::ICore3> createTestCore()
{
   return materia::createCore({"/home/snake/materia.db"});
}

class CoreTest
{
public:
   CoreTest()
   : mCore(createTestCore())
   {
      
   }

protected:
   std::shared_ptr<materia::ICore3> mCore;
};

BOOST_FIXTURE_TEST_CASE( NewDayTest, CoreTest ) 
{  
   mCore->onNewDay();
}

BOOST_FIXTURE_TEST_CASE( NewWeekTest, CoreTest ) 
{  
   mCore->onNewWeek();
}