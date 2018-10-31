#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <Core/ICore.hpp>
#include <Core/IJournal.hpp>

#include <thread>

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
   {
      system("rm Test.db");
      mCore = materia::createCore({"Test.db"});
      mJournal = &mCore->getJournal();

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

      mAnimalsId = mJournal->insertFolder(materia::Id::Invalid, "animals");
      mMamalsId = mJournal->insertFolder(mAnimalsId, "mamals");
      mSapiensId = mJournal->insertPage(mMamalsId, "sapiens", "<li>human</li><li>chimpanze</li><li>dolphin</li>");
      mNonSapiensId = mJournal->insertPage(mMamalsId, "non-sapiens", "<li>camel</li><li>cow</li><li>elephant</li>");
      mInsectsId = mJournal->insertPage(mAnimalsId, "insects", "<li>ant</li><li>bug</li><li>spider</li>");
      mColorsPageId = mJournal->insertPage(materia::Id::Invalid, "colors", "<li>red</li><li>green</li><li>blue</li>");
      mEmptyFolderId = mJournal->insertFolder(materia::Id::Invalid, "emptyFolder");

      mIndexSize = 7;
   }

   std::shared_ptr<materia::ICore> mCore;
   materia::IJournal* mJournal;
   
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
      mJournal->deleteItem(mEmptyFolderId);
      BOOST_CHECK_EQUAL(mIndexSize - 1, mJournal->getIndex().size());
   }

   //2. delete page
   {
      mJournal->deleteItem(mColorsPageId);
      BOOST_CHECK_EQUAL(mIndexSize - 2, mJournal->getIndex().size());
      BOOST_CHECK(!mJournal->getPage(mColorsPageId));
   }

   //3. delete contained folder -> results in pages deleted
   {
      mJournal->deleteItem(mMamalsId);
      BOOST_CHECK_EQUAL(mIndexSize - 5, mJournal->getIndex().size());
      BOOST_CHECK(!mJournal->getPage(mSapiensId));
      BOOST_CHECK(!mJournal->getPage(mNonSapiensId));
      BOOST_CHECK(mJournal->getPage(mInsectsId));
   }
   //4. delete all
   {
      mJournal->deleteItem(mAnimalsId);
      BOOST_CHECK_EQUAL(0, mJournal->getIndex().size());
   }
}

BOOST_FIXTURE_TEST_CASE( UpdateFolder_NoReparent, JournalTest ) 
{
   using namespace std::chrono_literals;

   auto index = mJournal->getIndex();
   auto item = *materia::find_by_id(index, mAnimalsId);
   item.title = "omg";

   std::this_thread::sleep_for(1s);
   mJournal->updateFolder(item);

   index = mJournal->getIndex();
   auto updatedItem = *materia::find_by_id(index, mAnimalsId);
   BOOST_CHECK_EQUAL(item.title, updatedItem.title);
   BOOST_CHECK(item.modified != updatedItem.modified);
}

BOOST_FIXTURE_TEST_CASE( UpdateFolder_SelfParent, JournalTest ) 
{
   auto index = mJournal->getIndex();
   auto item = *materia::find_by_id(index, mMamalsId);
   item.parentFolderId = mMamalsId;

   mJournal->updateFolder(item);

   index = mJournal->getIndex();
   auto updatedItem = *materia::find_by_id(index, mMamalsId);
   BOOST_CHECK(item.parentFolderId != updatedItem.parentFolderId);
   BOOST_CHECK_EQUAL(item.modified, updatedItem.modified);
}

BOOST_FIXTURE_TEST_CASE( UpdatePage, JournalTest ) 
{
   using namespace std::chrono_literals;

   auto index = mJournal->getIndex();
   auto oldPageIndexItem = *materia::find_by_id(index, mColorsPageId);

   auto page = *mJournal->getPage(mColorsPageId);
   page.title = "omg";
   page.content = "other_content";

   std::this_thread::sleep_for(1s);
   mJournal->updatePage(page);

   index = mJournal->getIndex();
   auto updatedItem = *materia::find_by_id(index, mColorsPageId);
   BOOST_CHECK_EQUAL(page.title, updatedItem.title);
   BOOST_CHECK(oldPageIndexItem.modified != updatedItem.modified);

   auto otherPage = *mJournal->getPage(mColorsPageId);
   BOOST_CHECK_EQUAL(page.content, otherPage.content);
}

BOOST_FIXTURE_TEST_CASE( GetIndex, JournalTest ) 
{
   auto index = mJournal->getIndex();

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
   BOOST_CHECK(!mJournal->getPage(mAnimalsId));
   auto page = *mJournal->getPage(mColorsPageId);

   BOOST_CHECK(materia::Id::Invalid != page.id);
   BOOST_CHECK_EQUAL("colors", page.title);
   BOOST_CHECK_EQUAL("<li>red</li><li>green</li><li>blue</li>", page.content);
}

BOOST_FIXTURE_TEST_CASE( SearchTest, JournalTest ) 
{
   auto result = mJournal->search("wrong");
   BOOST_CHECK(result.empty());

   result = mJournal->search("ph");
   BOOST_CHECK_EQUAL(2, result.size());
   if(result[0].pageId == mSapiensId)
   {
      BOOST_CHECK_EQUAL(mNonSapiensId, result[1].pageId);
      BOOST_CHECK_EQUAL(39, result[0].position);
      BOOST_CHECK_EQUAL(33, result[1].position);
   }
   else if(result[1].pageId == mSapiensId)
   {
      BOOST_CHECK_EQUAL(mNonSapiensId, result[0].pageId);
      BOOST_CHECK_EQUAL(39, result[1].position);
      BOOST_CHECK_EQUAL(33, result[0].position);
   }
   else
   {
      BOOST_CHECK(false);
   }

   result = mJournal->search("re");
   BOOST_CHECK_EQUAL(2, result.size());
   BOOST_CHECK_EQUAL(mColorsPageId, result[0].pageId);
   BOOST_CHECK_EQUAL(mColorsPageId, result[1].pageId);
   BOOST_CHECK_EQUAL(4, result[0].position);
   BOOST_CHECK_EQUAL(17, result[1].position);
}

}