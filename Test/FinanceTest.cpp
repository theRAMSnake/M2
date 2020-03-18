#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include "boost/date_time/gregorian/gregorian.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <Core/ICore.hpp>
#include <Core/IFinance.hpp>

static std::shared_ptr<materia::ICore> createTestCore()
{
   system("rm Test.db");
   return materia::createCore({"Test.db"});
}

class FinanceTest
{
public:
   FinanceTest()
   : mCore(createTestCore())
   , mFinance(mCore->getFinance())
   {
      
   }

protected:
   std::shared_ptr<materia::ICore> mCore;
   materia::IFinance& mFinance;
};

BOOST_FIXTURE_TEST_CASE( AddDeleteCategory, FinanceTest ) 
{  
   BOOST_CHECK(mFinance.addCategory({materia::Id::Invalid, "text"}) != materia::Id::Invalid);

   auto items = mFinance.getCategories();
   BOOST_CHECK_EQUAL(1, items.size());
   BOOST_CHECK_EQUAL("text", items[0].name);

   mFinance.removeCategory(items[0].id);
   
   items = mFinance.getCategories();
   BOOST_CHECK_EQUAL(0, items.size());
}

BOOST_FIXTURE_TEST_CASE( EditCategory, FinanceTest ) 
{
   materia::Id newId = mFinance.addCategory({materia::Id::Invalid, "text"});
   BOOST_CHECK(newId != materia::Id::Invalid);

   mFinance.replaceCategory({newId, "other_text"});

   auto items = mFinance.getCategories();
   BOOST_CHECK_EQUAL(1, items.size());
   BOOST_CHECK_EQUAL("other_text", items[0].name);
   BOOST_CHECK_EQUAL(newId, items[0].id);
}

BOOST_FIXTURE_TEST_CASE( AddDeleteSpendingEvent, FinanceTest ) 
{  
   BOOST_CHECK(mFinance.addEvent({materia::Id::Invalid, materia::Id::Invalid, materia::EventType::Spending, "details", 100, 5}) != materia::Id::Invalid);

   auto items = mFinance.queryEvents(0, 10);
   BOOST_CHECK_EQUAL(1, items.size());
   BOOST_CHECK_EQUAL("details", items[0].details);
   BOOST_CHECK_EQUAL(materia::Id::Invalid, items[0].categoryId);
   BOOST_CHECK_EQUAL(5, items[0].timestamp);
   BOOST_CHECK_EQUAL(100, items[0].amountEuroCents);

   mFinance.removeEvent(items[0].eventId);
   
   items = mFinance.queryEvents(0, 10);
   BOOST_CHECK_EQUAL(0, items.size());
}

BOOST_FIXTURE_TEST_CASE( EditSpendingEvent, FinanceTest )
{
   materia::Id newId = mFinance.addEvent({materia::Id::Invalid, materia::Id::Invalid, materia::EventType::Spending, "details", 100, 5});
   BOOST_CHECK(newId != materia::Id::Invalid);

   mFinance.replaceEvent({newId, materia::Id("5"), materia::EventType::Spending, "other_details", 200, 6});

   auto items = mFinance.queryEvents(0, 10);
   BOOST_CHECK_EQUAL(1, items.size());
   BOOST_CHECK_EQUAL("other_details", items[0].details);
   BOOST_CHECK_EQUAL(materia::Id("5"), items[0].categoryId);
   BOOST_CHECK_EQUAL(6, items[0].timestamp);
   BOOST_CHECK_EQUAL(200, items[0].amountEuroCents);
}

BOOST_FIXTURE_TEST_CASE( QuerySpendingEvent, FinanceTest ) 
{  
   materia::Id newId = mFinance.addEvent({materia::Id::Invalid, materia::Id::Invalid, materia::EventType::Spending,"details", 100, 5});
   materia::Id newId2 = mFinance.addEvent({materia::Id::Invalid, materia::Id::Invalid, materia::EventType::Spending, "details", 100, 10});
   materia::Id newId3 = mFinance.addEvent({materia::Id::Invalid, materia::Id::Invalid, materia::EventType::Spending, "details", 100, 15});

   {
      auto items = mFinance.queryEvents(0, 9);
      BOOST_CHECK_EQUAL(1, items.size());
      BOOST_CHECK_EQUAL(newId, items[0].eventId);
   }
   {
      auto items = mFinance.queryEvents(0, 20);
      BOOST_CHECK_EQUAL(3, items.size());
      BOOST_CHECK_EQUAL(newId, items[0].eventId);
      BOOST_CHECK_EQUAL(newId2, items[1].eventId);
      BOOST_CHECK_EQUAL(newId3, items[2].eventId);
   }
   {
      auto items = mFinance.queryEvents(20, 29);
      BOOST_CHECK_EQUAL(0, items.size());
   }
}