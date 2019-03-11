#define BOOST_TEST_DYN_LINK
#include <boost/test/included/unit_test.hpp>
#include <boost/filesystem.hpp>
#include <Core/ICore.hpp>
#include <Core/IInbox.hpp>


class InboxTest
{
public:
   InboxTest()
   {
      system("rm Test.db");
      mCore = materia::createCore({"Test.db"});
      mInbox = &mCore->getInbox();
   }

protected:
   bool isItemsConsistent()
   {
      return mInbox->get().empty();
   }

   std::shared_ptr<materia::ICore> mCore;
   materia::IInbox* mInbox;
};

BOOST_FIXTURE_TEST_CASE( AddDeleteInbox, InboxTest ) 
{  
   BOOST_CHECK(mInbox->add({materia::Id::Invalid, "text"}) != materia::Id::Invalid);

   auto items = mInbox->get();
   BOOST_CHECK_EQUAL(1, items.size());
   BOOST_CHECK_EQUAL("text", items[0].text);

   mInbox->remove(items[0].id);
   BOOST_CHECK(isItemsConsistent());
}

BOOST_FIXTURE_TEST_CASE( DeleteWrongInbox, InboxTest ) 
{
   mInbox->remove(materia::Id("wrong"));
   BOOST_CHECK(isItemsConsistent());
}

BOOST_FIXTURE_TEST_CASE( EditInbox, InboxTest ) 
{
   materia::Id newId = mInbox->add({materia::Id::Invalid, "text"});
   BOOST_CHECK(newId != materia::Id::Invalid);

   mInbox->replace({newId, "other_text"});

   auto items = mInbox->get();
   BOOST_CHECK_EQUAL(1, items.size());
   BOOST_CHECK_EQUAL("other_text", items[0].text);
   BOOST_CHECK_EQUAL(newId, items[0].id);

   mInbox->remove(items[0].id);
   BOOST_CHECK(isItemsConsistent());
}