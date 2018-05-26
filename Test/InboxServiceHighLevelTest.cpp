#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <messages/inbox.pb.h>
#include <Client/MateriaClient.hpp>
#include <Client/IInbox.hpp>
#include <Client/IContainer.hpp>
#include <boost/filesystem.hpp>

#include "TestHelpers.hpp"


class InboxTest
{
public:
   InboxTest()
   : mClient("test")
   , mService(mClient.getInbox())
   {
      mClient.getContainer().deleteContainer("inbox");
      mClient.getContainer().addContainer({"inbox"});
   }

protected:
   bool isItemsConsistent()
   {
      return mService.getItems().empty();
   }

   materia::MateriaClient mClient;
   materia::IInbox& mService;
};

BOOST_FIXTURE_TEST_CASE( AddDeleteInbox, InboxTest ) 
{  
   BOOST_CHECK(mService.insertItem({materia::Id::Invalid, "text"}) != materia::Id::Invalid);

   auto items = mService.getItems();
   BOOST_CHECK_EQUAL(1, items.size());
   BOOST_CHECK_EQUAL("text", items[0].text);

   BOOST_CHECK(mService.deleteItem(items[0].id));
   BOOST_CHECK(isItemsConsistent());
}

BOOST_FIXTURE_TEST_CASE( DeleteWrongInbox, InboxTest ) 
{
   mService.deleteItem(materia::Id("wrong"));
   BOOST_CHECK(isItemsConsistent());
}

BOOST_FIXTURE_TEST_CASE( EditInbox, InboxTest ) 
{
   materia::Id newId = mService.insertItem({materia::Id::Invalid, "text"});
   BOOST_CHECK(newId != materia::Id::Invalid);

   BOOST_CHECK(mService.replaceItem({newId, "other_text"}));

   auto items = mService.getItems();
   BOOST_CHECK_EQUAL(1, items.size());
   BOOST_CHECK_EQUAL("other_text", items[0].text);
   BOOST_CHECK_EQUAL(newId, items[0].id);

   BOOST_CHECK(mService.deleteItem(items[0].id));
   BOOST_CHECK(isItemsConsistent());
}