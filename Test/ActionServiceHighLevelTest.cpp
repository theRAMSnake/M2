#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <messages/actions.pb.h>
#include <boost/filesystem.hpp>

#include <mongocxx/instance.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>

#include <Client//MateriaClient.hpp>

#include "TestHelpers.hpp"

class ActionsTest
{
public:
   ActionsTest()
   : mClient("test")
   , mService(mClient.getActions())
   {
      mongocxx::instance instance{}; 
      mongocxx::client client{mongocxx::uri{}};

      client["materia"].drop();
   }

protected:

   materia::MateriaClient mClient;
   materia::Actions& mService;
};

namespace std
{
   std::ostream& operator << (std::ostream& str, const materia::ActionType& actionType)
   {
      std::string result;

      switch(actionType)
      {
         case materia::ActionType::Task:
            result = "task";
            break;

         case materia::ActionType::Group:
            result = "group";
            break;

         default:
            result = "???";
            break;
      }

      str << result;

      return str;
   }

   std::ostream& operator << (std::ostream& str, const materia::ActionItem& actionItem)
   {
      str << "[" << actionItem.id << ", " << actionItem.parentId << ", " << actionItem.title << ", " 
       << actionItem.description << ", " << actionItem.type << "]";
      return str;
   }
}

template<class T>
int CompareById (const T& a, const T& b)
{
   return a.id.getGuid().compare(b.id.getGuid());
}

template<class T>
void testSortAndCompare(const std::vector<T>& a, const std::vector<T>& b)
{
   auto sortedA = a;
   auto sortedB = b;

   std::sort(sortedA.begin(), sortedA.end(), CompareById<T>);
   std::sort(sortedB.begin(), sortedB.end(), CompareById<T>);

   BOOST_CHECK_EQUAL_COLLECTIONS(sortedA.begin(), sortedA.end(), 
      sortedB.begin(), sortedB.end());
}

BOOST_FIXTURE_TEST_CASE( AddDeleteAction_Parentless, ActionsTest ) 
{
   auto id1 = mService.insertItem({materia::Id::Invalid, materia::Id::Invalid, "text", "description", materia::ActionType::Task});
   auto id2 = mService.insertItem({materia::Id::Invalid, materia::Id::Invalid, "text2", "description2", materia::ActionType::Task});

   BOOST_CHECK(id1 != materia::Id::Invalid);
   BOOST_CHECK(id2 != materia::Id::Invalid);
   BOOST_CHECK(id1 != id2);

   auto rootItems = mService.getRootItems();
   BOOST_CHECK_EQUAL(2, rootItems.size());

   testSortAndCompare(rootItems, {
      {id1, materia::Id::Invalid, "text", "description", materia::ActionType::Task}, 
      {id2, materia::Id::Invalid, "text2", "description2", materia::ActionType::Task}});

   BOOST_CHECK(mService.deleteItem(id1));

   rootItems = mService.getRootItems();
   BOOST_CHECK_EQUAL(1, rootItems.size());

   BOOST_CHECK_EQUAL(rootItems[0].id, id2);

   BOOST_CHECK(mService.deleteItem(id2));

   rootItems = mService.getRootItems();
   BOOST_CHECK(rootItems.empty());
}

BOOST_FIXTURE_TEST_CASE( AddDeleteAction_Parented, ActionsTest ) 
{
   auto parentId1 = mService.insertItem({materia::Id::Invalid, materia::Id::Invalid, "text", "description", materia::ActionType::Group});
   auto childId1 = mService.insertItem({materia::Id::Invalid, parentId1, "text2", "description2", materia::ActionType::Task});
   auto childId2 = mService.insertItem({materia::Id::Invalid, parentId1, "text2", "description2", materia::ActionType::Task});
   auto nonChildId = mService.insertItem({materia::Id::Invalid, materia::Id::Invalid, "text3", "description3", materia::ActionType::Task});

   auto rootItems = mService.getRootItems();
   BOOST_CHECK_EQUAL(2, rootItems.size());
   testSortAndCompare(rootItems, {
      {parentId1, materia::Id::Invalid, "text", "description", materia::ActionType::Group}, 
      {nonChildId, materia::Id::Invalid, "text3", "description3", materia::ActionType::Task}});

   auto children = mService.getChildren(parentId1);
   BOOST_CHECK_EQUAL(2, children.size());
   testSortAndCompare(children, {
      {childId1, parentId1, "text2", "description2", materia::ActionType::Task},
      {childId2, parentId1, "text2", "description2", materia::ActionType::Task}});
      
   BOOST_CHECK(mService.deleteItem(parentId1));

   rootItems = mService.getRootItems();
   BOOST_CHECK_EQUAL(1, rootItems.size());
   BOOST_CHECK_EQUAL(rootItems[0].id, nonChildId);

   children = mService.getChildren(parentId1);
   BOOST_CHECK(children.empty());

   BOOST_CHECK(mService.deleteItem(nonChildId));

   rootItems = mService.getRootItems();
   BOOST_CHECK(rootItems.empty());
}

BOOST_FIXTURE_TEST_CASE( DeleteWrongAction, ActionsTest ) 
{
   BOOST_CHECK(!mService.deleteItem(materia::Id("dhjfhksd")));
}

BOOST_FIXTURE_TEST_CASE( EditWrongAction, ActionsTest ) 
{
   BOOST_CHECK(!mService.replaceItem({materia::Id::Invalid, materia::Id("dfsdfs"), "text2", "description2", materia::ActionType::Task}));
}

BOOST_FIXTURE_TEST_CASE( EditAction_NoReparent, ActionsTest ) 
{
   auto parentId = mService.insertItem({materia::Id::Invalid, materia::Id::Invalid, "text", "description", materia::ActionType::Group});
   BOOST_CHECK(mService.replaceItem({parentId, materia::Id::Invalid, "other_title", "other_description", materia::ActionType::Task}));

   auto rootItems = mService.getRootItems();
   BOOST_CHECK_EQUAL(1, rootItems.size());
   BOOST_CHECK_EQUAL(rootItems[0], materia::ActionItem({parentId, materia::Id::Invalid, "other_title", "other_description", materia::ActionType::Task}));
}

BOOST_FIXTURE_TEST_CASE( EditAction_Reparent, ActionsTest ) 
{  
   auto parentId1 = mService.insertItem({materia::Id::Invalid, materia::Id::Invalid, "text", "description", materia::ActionType::Group});
   auto parentId2 = mService.insertItem({materia::Id::Invalid, materia::Id::Invalid, "text", "description", materia::ActionType::Group});
   auto childId = mService.insertItem({materia::Id::Invalid, parentId1, "text2", "description2", materia::ActionType::Task});
   
   BOOST_CHECK(mService.replaceItem({childId, parentId2, "other_title", "other_description", materia::ActionType::Task}));

   auto children = mService.getChildren(parentId1);
   BOOST_CHECK(children.empty());

   children = mService.getChildren(parentId2);
   BOOST_CHECK_EQUAL(1, children.size());
   BOOST_CHECK_EQUAL(children[0], materia::ActionItem({childId, parentId2, "other_title", "other_description", materia::ActionType::Task}));
}

BOOST_FIXTURE_TEST_CASE( EditAction_Deparent, ActionsTest ) 
{
   auto parentId = mService.insertItem({materia::Id::Invalid, materia::Id::Invalid, "text", "description", materia::ActionType::Group});
   auto childId = mService.insertItem({materia::Id::Invalid, parentId, "text2", "description2", materia::ActionType::Task});
   
   BOOST_CHECK(mService.replaceItem({childId, materia::Id::Invalid, "other_title", "other_description", materia::ActionType::Task}));

   auto children = mService.getChildren(parentId);
   BOOST_CHECK(children.empty());

   auto rootItems = mService.getRootItems();
   BOOST_CHECK_EQUAL(2, rootItems.size());
   testSortAndCompare(rootItems, 
      {{parentId, materia::Id::Invalid, "text", "description", materia::ActionType::Group},
       {childId, materia::Id::Invalid, "other_title", "other_description", materia::ActionType::Task}});
}

BOOST_FIXTURE_TEST_CASE( EditAction_WrongParent, ActionsTest ) 
{
   auto parentId = mService.insertItem({materia::Id::Invalid, materia::Id::Invalid, "text", "description", materia::ActionType::Group});
   materia::ActionItem item {parentId, materia::Id("sfdfd"), "other_title", "other_description", materia::ActionType::Task};
   BOOST_CHECK(!mService.replaceItem(item));

   auto rootItems = mService.getRootItems();
   BOOST_CHECK_EQUAL(1, rootItems.size());
   BOOST_CHECK_EQUAL(rootItems[0], materia::ActionItem({parentId, materia::Id::Invalid, "text", "description", materia::ActionType::Group}));

   BOOST_CHECK(!mService.replaceItem(materia::ActionItem({parentId, parentId, "other_title", "other_description", materia::ActionType::Task})));
   rootItems = mService.getRootItems();
   BOOST_CHECK_EQUAL(1, rootItems.size());
   BOOST_CHECK_EQUAL(rootItems[0], materia::ActionItem({parentId, materia::Id::Invalid, "text", "description", materia::ActionType::Group}));
}

BOOST_FIXTURE_TEST_CASE( AddAction_WrongParent, ActionsTest ) 
{  
   materia::ActionItem item = {materia::Id::Invalid, materia::Id("sdgdgdgs"), "text", "description", materia::ActionType::Group};
   BOOST_CHECK(materia::Id::Invalid == mService.insertItem(item));
}