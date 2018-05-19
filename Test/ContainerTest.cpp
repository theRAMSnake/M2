#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <boost/lexical_cast.hpp>
#include <Client/MateriaClient.hpp>
#include <Client/IContainer.hpp>
#include "TestHelpers.hpp"
#include "TestEventListener.hpp"
#include <fstream>
#include <sqlite3.h>

namespace std
{
std::ostream& operator << (std::ostream& str, const materia::ContainerItem& containerItem)
{
   str << "[" << containerItem.id << ", " << containerItem.content << ", BlobSize: " 
      << containerItem.blob.size() << "]";
   return str;
}
}

class ContainerTest
{
public:
   ContainerTest()
   : mClient("test")
   , mEventListener(mClient.getEvents())
   , mService(mClient.getContainer())
   {
      cleanUp();
      //Test data - 4 containers, (5 items) (3 items, 1 blob, private)  (10 blobs) (empty)
      {
         mService.addContainer({"con1", true});

         std::vector<materia::ContainerItem> items;
         items.push_back({materia::Id::Invalid, "item1"});
         items.push_back({materia::Id::Invalid, "item2"});
         items.push_back({materia::Id::Invalid, "item3"});
         items.push_back({materia::Id::Invalid, "item4"});
         items.push_back({materia::Id::Invalid, "item5"});

         setIds(mService.insertItems("con1", items), items);
         mInsertedItems.insert(std::make_pair("con1", items));
      }
      {
         mService.addContainer({"con2", false});
         std::vector<materia::ContainerItem> items;
         items.push_back({materia::Id::Invalid, "item1"});
         items.push_back({materia::Id::Invalid, "item2"});
         items.push_back({materia::Id::Invalid, "item3", {0x15, 0x15, 0x15, 0x15}});

         setIds(mService.insertItems("con2", items), items);
         mInsertedItems.insert(std::make_pair("con2", items));
      }
      {
         mService.addContainer({"con3", true});
         std::vector<materia::ContainerItem> items;
         items.push_back({materia::Id::Invalid, "item1", {0x10, 0x10, 0x10, 0x10}});
         items.push_back({materia::Id::Invalid, "item2", {0x11, 0x11, 0x11, 0x11}});
         items.push_back({materia::Id::Invalid, "item3", {0x12, 0x12, 0x12, 0x12}});
         items.push_back({materia::Id::Invalid, "item4", {0x13, 0x12, 0x12, 0x12}});
         items.push_back({materia::Id::Invalid, "item5", {0x14, 0x12, 0x12, 0x12}});
         items.push_back({materia::Id::Invalid, "item6", {0x15, 0x12, 0x12, 0x12}});
         items.push_back({materia::Id::Invalid, "item7", {0x16, 0x12, 0x12, 0x12}});
         items.push_back({materia::Id::Invalid, "item8", {0x17, 0x12, 0x12, 0x12}});
         items.push_back({materia::Id::Invalid, "item9", {0x18, 0x12, 0x12, 0x12}});
         items.push_back({materia::Id::Invalid, "item10", {0x19, 0x12, 0x12, 0x12}});

         setIds(mService.insertItems("con3", items), items);
         mInsertedItems.insert(std::make_pair("con3", items));
      }
      {
         mService.addContainer({"con4", true});
      }

   }

protected:

   void cleanUp()
   {
      mService.deleteContainer("con1");
      mService.deleteContainer("con2");
      mService.deleteContainer("con3");
      mService.deleteContainer("con4");
      mService.deleteContainer("someCont");
      mService.deleteContainer("conSlot");
   }

   void setIds(const std::vector<materia::Id>& ids, std::vector<materia::ContainerItem>& items)
   {
      for(std::size_t i = 0; i < items.size(); ++i)
      {
         items[i].id = ids[i];
      }
   }

   std::vector<materia::Id> getIds(std::vector<materia::ContainerItem>& items)
   {
      std::vector<materia::Id> result;
      result.resize(items.size());
      std::transform(items.begin(), items.end(), result.begin(), [] (auto x)-> auto { return x.id; } );

      return result;
   }

   materia::MateriaClient mClient;
   TestEventListener mEventListener;
   materia::IContainer& mService;
   std::map<std::string, std::vector<materia::ContainerItem>> mInsertedItems;
};

BOOST_FIXTURE_TEST_CASE( AddContainer, ContainerTest )  
{
   BOOST_CHECK(mService.addContainer({"someCont", true}));
   BOOST_CHECK(!mService.addContainer({"", true}));
   BOOST_CHECK(!mService.addContainer({"someCont", true}));

   auto conts = mService.getPublicContainers();
   BOOST_CHECK(std::find(conts.begin(), conts.end(), materia::ContainerDefinition{"someCont", true}) != conts.end());
   BOOST_CHECK(std::find(conts.begin(), conts.end(), materia::ContainerDefinition{"", true}) == conts.end());

   BOOST_CHECK(mEventListener.hasEvent<materia::EventType::ContainerUpdated>("someCont"));
}

BOOST_FIXTURE_TEST_CASE( GetContainers, ContainerTest )  
{
   auto conts = mService.getPublicContainers();
   BOOST_CHECK_EQUAL(3, conts.size());
   BOOST_CHECK(std::find(conts.begin(), conts.end(), materia::ContainerDefinition{"con1", true}) != conts.end());
   BOOST_CHECK(std::find(conts.begin(), conts.end(), materia::ContainerDefinition{"con2", true}) == conts.end());
   BOOST_CHECK(std::find(conts.begin(), conts.end(), materia::ContainerDefinition{"con3", true}) != conts.end());
}

BOOST_FIXTURE_TEST_CASE( DeleteContainer, ContainerTest )  
{
   BOOST_CHECK(!mService.deleteContainer(""));
   BOOST_CHECK_EQUAL(3, mService.getPublicContainers().size());

   BOOST_CHECK(!mService.deleteContainer("notexist"));
   BOOST_CHECK_EQUAL(3, mService.getPublicContainers().size());

   BOOST_CHECK(mService.deleteContainer("con2"));
   BOOST_CHECK_EQUAL(3, mService.getPublicContainers().size());
   {
      auto conts = mService.getPublicContainers();
      BOOST_CHECK(std::find(conts.begin(), conts.end(), materia::ContainerDefinition{"con1", true}) != conts.end());
      BOOST_CHECK(std::find(conts.begin(), conts.end(), materia::ContainerDefinition{"con2", true}) == conts.end());
      BOOST_CHECK(std::find(conts.begin(), conts.end(), materia::ContainerDefinition{"con3", true}) != conts.end());
   }

   BOOST_CHECK(mService.deleteContainer("con1"));
   BOOST_CHECK(mService.deleteContainer("con3"));
   BOOST_CHECK(mService.deleteContainer("con4"));
   BOOST_CHECK_EQUAL(0, mService.getPublicContainers().size());

   BOOST_CHECK(mEventListener.hasEvent<materia::EventType::ContainerUpdated>("con1"));
   BOOST_CHECK(mEventListener.hasEvent<materia::EventType::ContainerUpdated>("con2"));
   BOOST_CHECK(mEventListener.hasEvent<materia::EventType::ContainerUpdated>("con3"));
   BOOST_CHECK(mEventListener.hasEvent<materia::EventType::ContainerUpdated>("con4"));
}

BOOST_FIXTURE_TEST_CASE( ClearContainer, ContainerTest )  
{
   BOOST_CHECK(mService.clearContainer("con1"));
   BOOST_CHECK(mService.getItems("con1").empty());

   BOOST_CHECK(mEventListener.hasEvent<materia::EventType::ContainerUpdated>("con1"));
}

BOOST_FIXTURE_TEST_CASE( GetItems, ContainerTest )  
{
   for(auto x : mInsertedItems)
   {
      auto items = mService.getItems(x.first);

      BOOST_CHECK_EQUAL_COLLECTIONS(x.second.begin(), x.second.end(), 
         items.begin(), items.end());
   }

   BOOST_CHECK(mService.getItems("sdfgdjkfg").empty());
}

BOOST_FIXTURE_TEST_CASE( InsertItems, ContainerTest )  
{
   std::vector<materia::ContainerItem> items;
   items.push_back({materia::Id::Invalid, "item1"});
   items.push_back({materia::Id::Invalid, "item2"});
   items.push_back({materia::Id::Invalid, "item3", {0x15, 0x15, 0x15, 0x15}});

   setIds(mService.insertItems("con4", items), items);

   auto newItems = mService.getItems("con4");

   BOOST_CHECK_EQUAL_COLLECTIONS(items.begin(), items.end(), 
         newItems.begin(), newItems.end());

   BOOST_CHECK(mEventListener.hasEvent<materia::EventType::ContainerUpdated>("con4"));
}

BOOST_FIXTURE_TEST_CASE( DeleteItems, ContainerTest )  
{
   BOOST_CHECK(!mService.deleteItems("invalid", {}));
   BOOST_CHECK(mService.deleteItems("con3", {}));
   BOOST_CHECK(mEventListener.hasEvent<materia::EventType::ContainerUpdated>("con3"));

   {
      auto items = mInsertedItems["con2"];
      BOOST_CHECK(mService.deleteItems("con2", getIds(items)));
      BOOST_CHECK_EQUAL(0, mService.getItems("con2").size());

      BOOST_CHECK(mEventListener.hasEvent<materia::EventType::ContainerUpdated>("con2"));
   }
   {
      auto items = mInsertedItems["con3"];
      decltype(items) halfItems(items.begin(), items.begin() + items.size() / 2);

      BOOST_CHECK(mService.deleteItems("con3", getIds(halfItems)));
      BOOST_CHECK_EQUAL(5, mService.getItems("con3").size());
   }
}

BOOST_FIXTURE_TEST_CASE( ReplaceItems, ContainerTest )  
{
   auto items = mInsertedItems["con3"];
   std::for_each(items.begin(), items.end(), [] (auto& x)-> auto { x.content += "___"; });

   BOOST_CHECK(mService.replaceItems("con3", items));
   BOOST_CHECK(mEventListener.hasEvent<materia::EventType::ContainerUpdated>("con3"));

   auto newItems = mService.getItems("con3");

   BOOST_CHECK_EQUAL_COLLECTIONS(items.begin(), items.end(), 
         newItems.begin(), newItems.end());

   BOOST_CHECK(!mService.replaceItems("con10", items));
}

BOOST_FIXTURE_TEST_CASE( Fetch_ContainerTest, ContainerTest )  
{
   auto result = mService.fetch();

   //result should be valid sqlite image

   {
      std::ofstream f("Fetch_ContainerTest", std::ios::out | std::ofstream::binary);
      std::copy(result.begin(), result.end(), std::ostreambuf_iterator<char>(f));
   }

   sqlite3* psqlite;
   BOOST_CHECK_EQUAL(SQLITE_OK, sqlite3_open("Fetch_ContainerTest", &psqlite));
}

BOOST_FIXTURE_TEST_CASE( ExecFunc, ContainerTest )  
{
   std::vector<materia::ContainerItem> items;
   items.push_back({materia::Id::Invalid, "5"});
   items.push_back({materia::Id::Invalid, "6"});
   items.push_back({materia::Id::Invalid, "7"});  

   mService.insertItems("con4", items);

   BOOST_CHECK_EQUAL(18, *mService.execFunc({materia::FuncType::Sum, "con4"}));
   BOOST_CHECK_EQUAL(3, *mService.execFunc({materia::FuncType::Count, "con4"}));

   BOOST_CHECK_EQUAL(0, *mService.execFunc({materia::FuncType::Sum, "con3"}));
   BOOST_CHECK_EQUAL(10, *mService.execFunc({materia::FuncType::Count, "con3"}));
}

BOOST_FIXTURE_TEST_CASE( ContainerSlot, ContainerTest )
{
   mService.addContainer({"conSlot"});

   {
      std::cout << ".";
      BOOST_CHECK(mService.getItems("conSlot").empty());

      std::cout << ".";
      materia::ContainerSlot slot1 = std::move(mService.acquireSlot("conSlot"));
      std::cout << ".";
      auto slot2 = mService.acquireSlot("conSlot");

      slot1.put("val");

      auto items = mService.getItems("conSlot"); 
      BOOST_CHECK_EQUAL(1, items.size());
      BOOST_CHECK_EQUAL("val", items[0].content);

      slot1.put("val2");
      items = mService.getItems("conSlot"); 
      BOOST_CHECK_EQUAL(1, items.size());
      BOOST_CHECK_EQUAL("val2", items[0].content);

      slot2.put("val2");
      items = mService.getItems("conSlot"); 
      BOOST_CHECK_EQUAL(2, items.size());
      BOOST_CHECK_EQUAL("val2", items[0].content);
      BOOST_CHECK_EQUAL("val2", items[1].content);

      {
         auto slot3 = mService.acquireSlot("conSlot");
         slot3.put("dsgssdg");

         items = mService.getItems("conSlot"); 
         BOOST_CHECK_EQUAL(3, items.size());
      }

      items = mService.getItems("conSlot"); 
      BOOST_CHECK_EQUAL(2, items.size());
   }

   auto items = mService.getItems("conSlot"); 
   BOOST_CHECK_EQUAL(0, items.size());
}  