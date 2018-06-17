#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <messages/actions.pb.h>
#include <Common/Utils.hpp>
#include <boost/filesystem.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <Client//MateriaClient.hpp>
#include <Client/IActions.hpp>
#include <Client/IContainer.hpp>
#include <Client/IStrategy.hpp>
#include <Client/ICalendar.hpp>

#include "TestHelpers.hpp"

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

namespace materia
{

class ActionsTest
{
public:
   ActionsTest()
   : mClient("test")
   , mService(mClient.getActions())
   {
      mService.clear();
      mClient.getStrategy().clear();
      mClient.getCalendar().clear();

      auto day = boost::gregorian::day_clock::local_day();
      day -= boost::gregorian::date_duration(1);

      //Create 5 free items, 5 calendar items (2 today) and 3 goals with tasks (2 focused)
      for(int i = 0; i < 5; ++i)
      {
         mService.insertItem({Id::Invalid, "item" + boost::lexical_cast<std::string>(i)});

         mClient.getCalendar().insertItem({Id::Invalid, "cal_item" + boost::lexical_cast<std::string>(i), day_to_time_t(day)});
         day += boost::gregorian::date_duration(1);
      }

      for(int i = 0; i < 3; ++i)
      {
         const bool focused = i != 0;
         auto id = mClient.getStrategy().addGoal(createGoal(focused));

         for(int j = 0; j < 3; ++j)
         {
            mClient.getStrategy().addTask(createTask(j, id));
         }
      }
   }

protected:
   materia::Goal createGoal(const bool focused)
   {
      materia::Goal g;
      g.name = "goal";
      g.notes = "notes of goal";
      g.iconId = materia::Id("some-icon-id");
      g.focused = focused;
      g.achieved = false;
      g.affinityId = materia::Id("some-affinity-id");

      return g;
   }

   materia::Task createTask(const int suffix, const materia::Id& parentGoalId = materia::Id::Invalid)
   {
      materia::Task t;
      t.done = false;
      t.name = "task" + boost::lexical_cast<std::string>(suffix);
      t.notes = "notes of task";
      t.parentGoalId = parentGoalId;
      t.iconId = materia::Id("some-icon-id");

      return t;
   }

   materia::MateriaClient mClient;
   materia::IActions& mService;
};

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

   std::sort(sortedA.begin(), sortedA.end());
   std::sort(sortedB.begin(), sortedB.end());

   BOOST_CHECK_EQUAL_COLLECTIONS(sortedA.begin(), sortedA.end(), 
      sortedB.begin(), sortedB.end());
}

BOOST_FIXTURE_TEST_CASE( Actions_GetItems, ActionsTest ) 
{
   auto items = mService.getItems();
   BOOST_CHECK_EQUAL(13, items.size());

   std::vector<std::string> expected {"item0", "item1", "item2", "item3", "item4",
      "cal_item0", "cal_item1",
      "task0", "task1", "task2",
      "task0", "task1", "task2"};

   std::vector<std::string> result (items.size());

   std::transform(items.begin(), items.end(), result.begin(), [](auto x)->auto{return x.title;});

   testSortAndCompare(expected, result);
}

BOOST_FIXTURE_TEST_CASE( Actions_DeleteItems, ActionsTest ) 
{
   auto items = mService.getItems();
   for(auto x : items)
   {
      BOOST_CHECK(mService.deleteItem(x.id));
   }

   auto newitems = mService.getItems();
   BOOST_CHECK(newitems.empty());

   BOOST_CHECK_EQUAL(3, mClient.getCalendar().next(0, 10).size());

   auto& strategy = mClient.getStrategy();
   auto goals = strategy.getGoals();

   for(auto g : goals)
   {
      if(g.focused)
      {
         auto tasks = std::get<0>(strategy.getGoalItems(g.id));

         for(auto t : tasks)
         {
            BOOST_CHECK(t.done);
         }
      }
   }
}

BOOST_FIXTURE_TEST_CASE( Actions_ReplaceItems, ActionsTest ) 
{
   auto items = mService.getItems();
   for(auto x : items)
   {
      const bool expectedReplaceResult = x.title[0] == 'i';
      x.title = "other";

      BOOST_CHECK_EQUAL(expectedReplaceResult, mService.replaceItem(x));

      if(expectedReplaceResult)
      {
         auto newItems = mService.getItems();
         auto newItem = *find_by_id(newItems, x.id);
         BOOST_CHECK_EQUAL(newItem, x);
      }
   }
}

}