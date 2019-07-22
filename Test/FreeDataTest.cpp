#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <Core/ICore.hpp>
#include <Core/IFreeData.hpp>

extern std::shared_ptr<materia::ICore> createTestCore();

class FreeDataTest
{
public:
   FreeDataTest()
   : mCore(createTestCore())
   , mFd(mCore->getFreeData())
   {
      
   }

protected:

   std::shared_ptr<materia::ICore> mCore;
   materia::IFreeData& mFd;
};

BOOST_FIXTURE_TEST_CASE( SetDeleteBlock, FreeDataTest ) 
{  
   mFd.set({"a", 1});
   mFd.set({"b", 2});
   mFd.set({"c", 3});

   auto items = mFd.get();
   BOOST_CHECK_EQUAL(3, items.size());
   BOOST_CHECK_EQUAL("a", items[0].name);
   BOOST_CHECK_EQUAL("b", items[1].name);
   BOOST_CHECK_EQUAL("c", items[2].name);

   BOOST_CHECK_EQUAL(1, items[0].value);
   BOOST_CHECK_EQUAL(2, items[1].value);
   BOOST_CHECK_EQUAL(3, items[2].value);

   mFd.remove("a");
   
   items = mFd.get();
   BOOST_CHECK_EQUAL(2, items.size());

   BOOST_CHECK_EQUAL("b", items[0].name);
   BOOST_CHECK_EQUAL("c", items[1].name);

   BOOST_CHECK_EQUAL(2, items[0].value);
   BOOST_CHECK_EQUAL(3, items[1].value);
}

BOOST_FIXTURE_TEST_CASE( IncrementBlock, FreeDataTest ) 
{
   mFd.set({"a", 1});
   mFd.set({"b", 2});
   mFd.set({"c", 4});

   mFd.increment("a", 5);
   mFd.increment("b", -5);
   mFd.increment("d", -5);

   auto items = mFd.get();
   BOOST_CHECK_EQUAL(3, items.size());
   BOOST_CHECK_EQUAL("c", items[0].name);
   BOOST_CHECK_EQUAL("a", items[1].name);
   BOOST_CHECK_EQUAL("b", items[2].name);

   BOOST_CHECK_EQUAL(4, items[0].value);
   BOOST_CHECK_EQUAL(6, items[1].value);
   BOOST_CHECK_EQUAL(-3, items[2].value);
}

BOOST_FIXTURE_TEST_CASE( InterpreterExecTest, FreeDataTest ) 
{
   
}

BOOST_FIXTURE_TEST_CASE( CheckExpressionTest, FreeDataTest ) 
{
   
}