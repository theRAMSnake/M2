#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <Core/ICore.hpp>
#include <Core/IReward.hpp>


class RewardTest
{
public:
   RewardTest()
   {
      system("rm Test.db");
      mCore = materia::createCore({"Test.db"});
      mReward = &mCore->getReward();
   }

protected:

   std::shared_ptr<materia::ICore> mCore;
   materia::IReward* mReward;
};

BOOST_FIXTURE_TEST_CASE( AddDeleteReward, RewardTest ) 
{  
   BOOST_CHECK(mReward->addPool({materia::Id::Invalid, "text", 0, 5}) != materia::Id::Invalid);

   auto items = mReward->getPools();
   BOOST_CHECK_EQUAL(1, items.size());
   BOOST_CHECK_EQUAL("text", items[0].name);
   BOOST_CHECK_EQUAL(0, items[0].amount);
   BOOST_CHECK_EQUAL(5, items[0].amountMax);

   mReward->removePool(items[0].id);

   items = mReward->getPools();
   BOOST_CHECK_EQUAL(0, items.size());
}

BOOST_FIXTURE_TEST_CASE( DeleteWrongReward, RewardTest ) 
{
   BOOST_CHECK(mReward->addPool({materia::Id::Invalid, "text", 0, 5}) != materia::Id::Invalid);

   mReward->removePool(materia::Id("wrong"));

   auto items = mReward->getPools();
   BOOST_CHECK_EQUAL(1, items.size());
}

BOOST_FIXTURE_TEST_CASE( EditReward, RewardTest ) 
{
   materia::Id newId = mReward->addPool({materia::Id::Invalid, "text", 0, 5});
   BOOST_CHECK(newId != materia::Id::Invalid);

   mReward->modifyPool({newId, "other_text", 1, 6});

   auto items = mReward->getPools();
   BOOST_CHECK_EQUAL(1, items.size());
   BOOST_CHECK_EQUAL("other_text", items[0].name);
   BOOST_CHECK_EQUAL(newId, items[0].id);
   BOOST_CHECK_EQUAL(1, items[0].amount);
   BOOST_CHECK_EQUAL(6, items[0].amountMax);
}

BOOST_FIXTURE_TEST_CASE( AddPoints, RewardTest ) 
{
   mReward->addPool({materia::Id::Invalid, "p1", 0, 5});
   mReward->addPool({materia::Id::Invalid, "p2", 3, 5});
   mReward->addPool({materia::Id::Invalid, "p3", 2, 5});

   mReward->addPoints(11);

   auto items = mReward->getPools();
   for(auto& p : items)
   {
      BOOST_CHECK_EQUAL(5, p.amount);
   }
}