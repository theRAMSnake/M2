#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <Core/ICore.hpp>
#include <Core/IStrategy_v2.hpp>

extern std::shared_ptr<materia::ICore> createTestCore();

class Strategy2Test
{
public:
   Strategy2Test()
   : mCore(createTestCore())
   , mStrategy(mCore->getStrategy_v2())
   {
      
   }

protected:

   std::shared_ptr<materia::ICore> mCore;
   materia::IStrategy_v2& mStrategy;
};

BOOST_FIXTURE_TEST_CASE( AddDeleteWatchItem, Strategy2Test ) 
{  
   BOOST_CHECK(mStrategy.addWatchItem({materia::Id::Invalid, "text"}) != materia::Id::Invalid);

   auto items = mStrategy.getWatchItems();
   BOOST_CHECK_EQUAL(1, items.size());
   BOOST_CHECK_EQUAL("text", items[0].text);

   mStrategy.removeWatchItem(items[0].id);
   
   items = mStrategy.getWatchItems();
   BOOST_CHECK_EQUAL(0, items.size());
}

BOOST_FIXTURE_TEST_CASE( EditWatchItem, Strategy2Test ) 
{
   materia::Id newId = mStrategy.addWatchItem({materia::Id::Invalid, "text"});
   BOOST_CHECK(newId != materia::Id::Invalid);

   mStrategy.replaceWatchItem({newId, "other_text"});

   auto items = mStrategy.getWatchItems();
   BOOST_CHECK_EQUAL(1, items.size());
   BOOST_CHECK_EQUAL("other_text", items[0].text);
   BOOST_CHECK_EQUAL(newId, items[0].id);
}