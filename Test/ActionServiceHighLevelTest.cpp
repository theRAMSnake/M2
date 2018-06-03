#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <messages/actions.pb.h>
#include <boost/filesystem.hpp>
#include <Client//MateriaClient.hpp>
#include <Client/IActions.hpp>
#include <Client/IContainer.hpp>

#include "TestHelpers.hpp"

class ActionsTest
{
public:
   ActionsTest()
   : mClient("test")
   , mService(mClient.getActions())
   {
      mService.clear();
   }

protected:

   materia::MateriaClient mClient;
   materia::IActions& mService;
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
      str << "[" << actionItem.id << ", " << actionItem.dataSourceId << ", " << actionItem.title << ", " 
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
   auto id1 = mService.insertItem({materia::Id::Invalid, "text", "description", materia::ActionType::Task, materia::Id::Invalid});
   auto id2 = mService.insertItem({materia::Id::Invalid, "text2", "description2", materia::ActionType::Task, materia::Id::Invalid});

   BOOST_CHECK(id1 != materia::Id::Invalid);
   BOOST_CHECK(id2 != materia::Id::Invalid);
   BOOST_CHECK(id1 != id2);

   auto rootItems = mService.getRootItems();
   BOOST_CHECK_EQUAL(2, rootItems.size());

   testSortAndCompare(rootItems, {
      {id1, "text", "description", materia::ActionType::Task, materia::Id::Invalid}, 
      {id2, "text2", "description2", materia::ActionType::Task, materia::Id::Invalid}});

   BOOST_CHECK(mService.deleteItem(id1));

   rootItems = mService.getRootItems();
   BOOST_CHECK_EQUAL(1, rootItems.size());

   BOOST_CHECK_EQUAL(rootItems[0].id, id2);

   BOOST_CHECK(mService.deleteItem(id2));

   rootItems = mService.getRootItems();
   BOOST_CHECK(rootItems.empty());
}

BOOST_FIXTURE_TEST_CASE( DeleteWrongAction, ActionsTest ) 
{
   BOOST_CHECK(!mService.deleteItem(materia::Id("dhjfhksd")));
}