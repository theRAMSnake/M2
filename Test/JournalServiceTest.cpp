#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <messages/journal.pb.h>
#include <boost/filesystem.hpp>
#include "TestServiceProvider.hpp"

#include <mongocxx/instance.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>

#include <boost/optional.hpp>
#include <Client/Id.hpp>

namespace std
{
   std::ostream& operator << (std::ostream& str, const materia::Id& id)
   {
      str << id.getGuid();
      return str;
   }
}

namespace 
{

struct TestItem
{
   std::string title;
   materia::Id id;
};

class JournalTest
{
public:
   JournalTest()
   {
      mongocxx::instance instance{}; 
      mongocxx::client client{mongocxx::uri{}};

      client["materia"].drop();

      fillSampleItems();
   }

protected:
   std::vector<TestItem> fillSampleItems()
   {
      mItems.push_back(addFolder("animals", materia::Id::Invalid));
      auto animalsId = mItems.back().id;

         mItems.push_back(addFolder("mamals", animalsId));
         auto mamalsId = mItems.back().id;

            mItems.push_back(addPage("sapiens", mamalsId, "<li>human</li><li>chimpanze</li><li>dolphin</li>"));
            mItems.push_back(addPage("non-sapiens", mamalsId, "<li>camel</li><li>cow</li><li>elephant</li>"));

         mItems.push_back(addPage("insects", animalsId, "<li>ant</li><li>bug</li><li>spider</li>"));

      mItems.push_back(addPage("colors", materia::Id::Invalid, "<li>red</li><li>green</li><li>blue</li>"));
      mItems.push_back(addFolder("emptyFolder", materia::Id::Invalid));

      mNumItemsInDatabase = 7;

      return mItems;
   }

   TestItem addFolder(const std::string& title, const materia::Id& parentId)
   {
      journal::InsertFolderParams item;
      item.set_title(title);
      item.mutable_folderid()->CopyFrom(parenttoProto(id));

      common::UniqueId id;
      mService.InsertFolder(nullptr, &item, &id, nullptr);

      return {title, materia::Id(id)};
   }

   TestItem addPage(const std::string& title, const materia::Id& parentId, const std::string& content)
   {
      journal::InsertPageParams item;
      item.set_title(title);
      item.mutable_folderid()->CopyFrom(parenttoProto(id));
      item.set_content(content);

      common::UniqueId id;
      mService.InsertPage(nullptr, &item, &id, nullptr);

      return {title, id};
   }

   unsigned int getNumItemsInDatabase() const
   {
      return loadIndex().items_size();
   }

   journal::Index loadIndex() const
   {
      journal::Index result;

      common::EmptyMessage empty;
      mService.GetIndex(nullptr, &empty, &result, nullptr);

      return result;
   }

   boost::optional<journal::IndexItem> getItemFromIndex(const materia::Id& id) const
   {
      boost::optional<journal::IndexItem> result;

      auto index = loadIndex();

      auto iter = std::find_if(index.items().begin(), index.items().end(), [&](auto a)->bool {return id == materia::Id(a.journalitem().id());});
      if(iter != index.items().end())
      {
         result = *iter;
      }

      return result;
   }

   boost::optional<journal::Page> getPage(const materia::Id& id) const
   {
      boost::optional<journal::Page> result;

      common::UniqueId matid(toProto(id));
      mService.GetPage(nullptr, &matid, &result.get(), nullptr);

      return result;
   }

   std::vector<TestItem> mItems;
   int mNumItemsInDatabase = 0;
   TestServiceProvider<journal::JournalService> mServiceProvider;
   journal::JournalService_Stub& mService = mServiceProvider.getService();
};

BOOST_FIXTURE_TEST_CASE( InsertFolder, JournalTest ) 
{
   {
      auto text = "testItem";
      auto id = addFolder(text, common::UniqueId()).id;

      BOOST_CHECK_EQUAL(mNumItemsInDatabase + 1, getNumItemsInDatabase());
      auto newItem = getItemFromIndex(id)->journalitem();

      BOOST_CHECK(newItem.folderid().guid().empty());
      BOOST_CHECK_EQUAL(id, materia::Id(newItem.id()));
      BOOST_CHECK_EQUAL(text, newItem.title());
   }
   {
      auto text = "testItem2";
      auto id = addFolder(text, mItems[0].id).id;

      BOOST_CHECK_EQUAL(mNumItemsInDatabase + 2, getNumItemsInDatabase());
      auto newItem = getItemFromIndex(id)->journalitem();

      BOOST_CHECK(newItem.folderid().guid().empty());
      BOOST_CHECK_EQUAL(id, materia::Id(newItem.id()));
      BOOST_CHECK_EQUAL(text, newItem.title());
   }
   {
      auto text = "testItem2";
      auto id = addFolder(text, materia::Id("random id")).id;

      BOOST_CHECK_EQUAL(mNumItemsInDatabase + 2, getNumItemsInDatabase());
      auto newItem = getItemFromIndex(id);

      BOOST_CHECK(!newItem);
   }
}

BOOST_FIXTURE_TEST_CASE( InsertPage, JournalTest ) 
{
   {
      auto text = "testItem";
      auto content = "<div>text</div>";
      auto id = addPage(text, common::UniqueId(), content).id;

      BOOST_CHECK_EQUAL(mNumItemsInDatabase + 1, getNumItemsInDatabase());
      auto newItem = getItemFromIndex(id)->journalitem();

      BOOST_CHECK(newItem.folderid().guid().empty());
      BOOST_CHECK_EQUAL(id, materia::Id(newItem.id()));
      BOOST_CHECK_EQUAL(text, newItem.title());

      auto page = getPage(id);
      BOOST_CHECK(page);
      BOOST_CHECK_EQUAL(content, page->content());
   }
   {
      auto text = "testItem2";
      auto content = "<div>text2</div>";
      auto id = addPage(text, mItems[0].id, content).id;

      BOOST_CHECK_EQUAL(mNumItemsInDatabase + 2, getNumItemsInDatabase());
      auto newItem = getItemFromIndex(id)->journalitem();

      BOOST_CHECK(newItem.folderid().guid().empty());
      BOOST_CHECK_EQUAL(id, materia::Id(newItem.id()));
      BOOST_CHECK_EQUAL(text, newItem.title());

      auto page = getPage(id);
      BOOST_CHECK(page);
      BOOST_CHECK_EQUAL(content, page->content());
   }
   {
      auto text = "testItem2";
      auto content = "<div>text2</div>";
      auto id = addPage(text, materia::Id("random id"), content).id;

      BOOST_CHECK_EQUAL(mNumItemsInDatabase + 2, getNumItemsInDatabase());
      auto newItem = getItemFromIndex(id);

      BOOST_CHECK(!newItem);

      auto page = getPage(id);
      BOOST_CHECK(!page);
   }
}

BOOST_FIXTURE_TEST_CASE( DeleteItem, JournalTest ) 
{
   //1. delete empty folder
   {
      /*auto iter = std::find_if(mItems.begin(), mItems.end(), [] (auto a) -> bool {return a.title == "emptyFolder";});
      auto id = iter->id;
      BOOST_REQUIRE(iter != mItems.end());

      BOOST_CHECK(deleteItem(id));

      BOOST_CHECK_EQUAL(mNumItemsInDatabase - 1, getNumItemsInDatabase());
      BOOST_CHECK(getItemFromIndex(id));*/
   }

   //2. delete empty page
   //3. delete contained folder
   //4. delete all
}

}