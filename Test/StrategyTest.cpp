#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <boost/lexical_cast.hpp>
#include <Client/MateriaClient.hpp>
#include <Client/Strategy.hpp>
#include "TestHelpers.hpp"
#include <fstream>
#include <sqlite3.h>

materia::Goal createGoal(const int suffix, const materia::Id& parentGoalId = materia::Id::Invalid)
{
   materia::Goal g;
   g.name = "goal" + boost::lexical_cast<std::string>(suffix);
   g.notes = "notes of goal";
   g.iconId = materia::Id("some-icon-id");
   g.focused = true;
   g.achiieved = false;
   g.affinityId = materia::Id("some-affinity-id");
   g.parentGoalId = parentGoalId;

   return g;
}

materia::Task createTask(const int suffix, const materia::Id& parentGoalId = materia::Id::Invalid)
{
   materia::Task t;
   t.done = true;
   t.count = 1;
   t.actionReference = materia::Id("some-action-id");
   t.calendarReference = materia::Id("some-calendar-id");
   t.name = "task" + boost::lexical_cast<std::string>(suffix);

   return t;
}

class StrategyTest
{
public:
   StrategyTest()
   : mClient("test")
   , mService(mClient.getStrategy())
   {
      cleanUp();
      //Test data - 4 gls (2, 3, 2 subgoals)(0, 0)(7, 0)(0, 9), 3 affinities, 2 measurements (value, func)
      {
         materia::Goal g = createGoal(1);
         g.id = mService.addGoal(g);
         mGoals.push_back(g);

         for(int i = 0; i < 2; ++i)
         {
            materia::Task t;
            t.done = true;
            t.count = 1;
            t.actionReference = materia::Id("some-action-id");
            t.calendarReference = materia::Id("some-calendar-id");
            t.name = "task" + boost::lexical_cast<std::string>(i);

         }
      }
      {
         materia::Goal g = createGoal(2);
         g.id = mService.addGoal(g);
         mGoals.push_back(g);
      }
      {
         materia::Goal g = createGoal(3, mGoals[0].id);
         g.id = mService.addGoal(g);
         mGoals.push_back(g);
      }
      {
         materia::Goal g = createGoal(4, mGoals[0].id);
         g.id = mService.addGoal(g);
         mGoals.push_back(g);
      }
   }

protected:

   void cleanUp()
   {
      mService.deleteContainer("goals");
      mService.deleteContainer("measurements");
      mService.deleteContainer("affinities");
   }

   materia::MateriaClient mClient;
   materia::Strategy& mService;
   std::vector<materia::Goal> mGoals;
};