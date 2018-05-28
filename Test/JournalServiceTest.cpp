#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <Client/Id.hpp>
#include <Client/MateriaClient.hpp>
#include <Client/IJournal.hpp>
#include <Client/IContainer.hpp>
#include "TestHelpers.hpp"

namespace std
{
std::ostream& operator << (std::ostream& str, const materia::JournalItem& item)
{
   str << "[" << item.id << ", " << item.parentFolderId << ", " << item.title << "]";
   return str;
}
}

namespace 
{

class JournalTest
{
public:
   JournalTest()
   : mClient("test")
   , mService(mClient.getJournal())
   {
      mClient.getJournal().clear();

      fillSampleItems();
   }

protected:
   void fillSampleItems()
   {
      /*
       * Structure as follows:
       * 
       * animals
       *    mamals
       *       sapiens
       *       non-sapiens
       *    insects
       * colors
       * emptyFolder
       */

      mAnimalsId = mService.insertFolder(materia::Id::Invalid, "animals");
      mMamalsId = mService.insertFolder(mAnimalsId, "mamals");
      mSapiensId = mService.insertPage(mMamalsId, "sapiens", "<li>human</li><li>chimpanze</li><li>dolphin</li>");
      mNonSapiensId = mService.insertPage(mMamalsId, "non-sapiens", "<li>camel</li><li>cow</li><li>elephant</li>");
      mInsectsId = mService.insertPage(mAnimalsId, "insects", "<li>ant</li><li>bug</li><li>spider</li>");
      mColorsPageId = mService.insertPage(materia::Id::Invalid, "colors", "<li>red</li><li>green</li><li>blue</li>");
      mEmptyFolderId = mService.insertFolder(materia::Id::Invalid, "emptyFolder");

      mIndexSize = 7;
   }

   materia::MateriaClient mClient;
   materia::IJournal& mService;
   
   std::size_t mIndexSize;

   materia::Id mAnimalsId;
   materia::Id mMamalsId;
   materia::Id mEmptyFolderId;
   materia::Id mColorsPageId;
   materia::Id mSapiensId;
   materia::Id mNonSapiensId;
   materia::Id mInsectsId;
};

BOOST_FIXTURE_TEST_CASE( DeleteItem, JournalTest ) 
{
   //1. delete empty folder
   {
      mService.deleteItem(mEmptyFolderId);
      BOOST_CHECK_EQUAL(mIndexSize - 1, mService.getIndex().size());
   }

   //2. delete page
   {
      mService.deleteItem(mColorsPageId);
      BOOST_CHECK_EQUAL(mIndexSize - 2, mService.getIndex().size());
      BOOST_CHECK(!mService.getPage(mColorsPageId));
   }

   //3. delete contained folder -> results in pages deleted
   {
      mService.deleteItem(mMamalsId);
      BOOST_CHECK_EQUAL(mIndexSize - 5, mService.getIndex().size());
      BOOST_CHECK(!mService.getPage(mSapiensId));
      BOOST_CHECK(!mService.getPage(mNonSapiensId));
      BOOST_CHECK(mService.getPage(mInsectsId));
   }
   //4. delete all
   {
      mService.deleteItem(mAnimalsId);
      BOOST_CHECK_EQUAL(0, mService.getIndex().size());
   }
}

BOOST_FIXTURE_TEST_CASE( UpdateFolder_NoReparent, JournalTest ) 
{
   auto index = mService.getIndex();
   auto item = *materia::find_by_id(index, mAnimalsId);
   item.title = "omg";

   std::cout << "\n: " << item.modified;

   BOOST_CHECK(mService.updateFolder(item));

   index = mService.getIndex();
   auto updatedItem = *materia::find_by_id(index, mAnimalsId);
   BOOST_CHECK_EQUAL(item.title, updatedItem.title);
   std::cout << "\n: " << item.modified << " " << updatedItem.modified << "\n"; 
   BOOST_CHECK(item.modified != updatedItem.modified);
}

BOOST_FIXTURE_TEST_CASE( UpdateFolder_SelfParent, JournalTest ) 
{
   auto index = mService.getIndex();
   auto item = *materia::find_by_id(index, mMamalsId);
   item.parentFolderId = mMamalsId;

   mService.updateFolder(item);

   index = mService.getIndex();
   auto updatedItem = *materia::find_by_id(index, mMamalsId);
   BOOST_CHECK(item.id != updatedItem.id);
   BOOST_CHECK_EQUAL(item.modified, updatedItem.modified);
}

BOOST_FIXTURE_TEST_CASE( UpdatePage, JournalTest ) 
{
   auto index = mService.getIndex();
   auto oldPageIndexItem = *materia::find_by_id(index, mColorsPageId);

   auto page = *mService.getPage(mColorsPageId);
   page.title = "omg";
   page.content = "other_content";

   mService.updatePage(page);

   index = mService.getIndex();
   auto updatedItem = *materia::find_by_id(index, mColorsPageId);
   BOOST_CHECK_EQUAL(page.title, updatedItem.title);
   BOOST_CHECK(oldPageIndexItem.modified != updatedItem.modified);

   auto otherPage = *mService.getPage(mColorsPageId);
   BOOST_CHECK_EQUAL(page.content, otherPage.content);
}

BOOST_FIXTURE_TEST_CASE( GetIndex, JournalTest ) 
{
   auto index = mService.getIndex();

   BOOST_CHECK_EQUAL(mIndexSize, index.size());

   {
      auto animals = *materia::find_by_id(index, mAnimalsId);
      BOOST_CHECK(!animals.isPage);
      BOOST_CHECK_EQUAL("animals", animals.title);
      BOOST_CHECK_EQUAL(materia::Id::Invalid, animals.parentFolderId);
   }
   {
      auto mamals = *materia::find_by_id(index, mMamalsId);
      BOOST_CHECK(!mamals.isPage);
      BOOST_CHECK_EQUAL("mamals", mamals.title);
      BOOST_CHECK_EQUAL(mAnimalsId, mamals.parentFolderId);
   }
   {
      auto sapiens = *materia::find_by_id(index, mSapiensId);
      BOOST_CHECK(sapiens.isPage);
      BOOST_CHECK_EQUAL("sapiens", sapiens.title);
      BOOST_CHECK_EQUAL(mMamalsId, sapiens.parentFolderId);
   }
   {
      auto item = *materia::find_by_id(index, mNonSapiensId);
      BOOST_CHECK(item.isPage);
      BOOST_CHECK_EQUAL("non-sapiens", item.title);
      BOOST_CHECK_EQUAL(mMamalsId, item.parentFolderId);
   }
   {
      auto item = *materia::find_by_id(index, mInsectsId);
      BOOST_CHECK(item.isPage);
      BOOST_CHECK_EQUAL("insects", item.title);
      BOOST_CHECK_EQUAL(mAnimalsId, item.parentFolderId);
   }
   {
      auto item = *materia::find_by_id(index, mColorsPageId);
      BOOST_CHECK(item.isPage);
      BOOST_CHECK_EQUAL("colors", item.title);
      BOOST_CHECK_EQUAL(materia::Id::Invalid, item.parentFolderId);
   }
   {
      auto item = *materia::find_by_id(index, mEmptyFolderId);
      BOOST_CHECK(!item.isPage);
      BOOST_CHECK_EQUAL("emptyFolder", item.title);
      BOOST_CHECK_EQUAL(materia::Id::Invalid, item.parentFolderId);
   }
}

BOOST_FIXTURE_TEST_CASE( GetPage, JournalTest ) 
{
   BOOST_CHECK(!mService.getPage(mAnimalsId));
   auto page = *mService.getPage(mColorsPageId);

   BOOST_CHECK_EQUAL(materia::Id::Invalid, page.id);
   BOOST_CHECK_EQUAL("colors", page.title);
   BOOST_CHECK_EQUAL("<li>red</li><li>green</li><li>blue</li>", page.content);
}

BOOST_FIXTURE_TEST_CASE( SearchTest, JournalTest ) 
{
   auto result = mService.search("wrong");
   BOOST_CHECK(result.empty());

   result = mService.search("co");
   BOOST_CHECK_EQUAL(2, result.size());
   BOOST_CHECK_EQUAL(mSapiensId, result[0].pageId);
   BOOST_CHECK_EQUAL(mColorsPageId, result[1].pageId);
   BOOST_CHECK_EQUAL(39, result[0].position);
   BOOST_CHECK_EQUAL(33, result[1].position);

   result = mService.search("re");
   BOOST_CHECK_EQUAL(2, result.size());
   BOOST_CHECK_EQUAL(mColorsPageId, result[0].pageId);
   BOOST_CHECK_EQUAL(mColorsPageId, result[1].pageId);
   BOOST_CHECK_EQUAL(4, result[0].position);
   BOOST_CHECK_EQUAL(17, result[1].position);
}

}