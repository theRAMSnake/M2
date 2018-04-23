#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <boost/lexical_cast.hpp>
#include <Client/MateriaClient.hpp>
#include <Client/Strategy.hpp>
#include "TestHelpers.hpp"
#include <fstream>
#include <sqlite3.h>

namespace std
{
   bool operator < (const materia::Id& a, const materia::Id& b)
   {
      return a.getGuid() < b.getGuid();
   }

   std::ostream& operator << (std::ostream& str, const materia::Goal& g)
   {
      str << "[" << g.id << ", " << g.parentGoalId << ", " 
         << g.name << ", " << g.notes << ", " << g.iconId << ", " << g.focused << ", " << g.achieved  << 
         ", " << g.affinityId << ", " << g.requiredGoals.size() << "]";
      return str;
   }
}

materia::Goal createGoal(const int suffix, const materia::Id& parentGoalId = materia::Id::Invalid)
{
   materia::Goal g;
   g.name = "goal" + boost::lexical_cast<std::string>(suffix);
   g.notes = "notes of goal";
   g.iconId = materia::Id("some-icon-id");
   g.focused = true;
   g.achieved = false;
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
   t.notes = "notes of task";
   t.parentGoalId = parentGoalId;
   t.iconId = materia::Id("some-icon-id");

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

      //Test data - 4 gls (2, 3, 2 subgoals)(0, 0)(7, 0)(0, 9), 3 affinities, 3 measurements (value, func)
      {
         materia::Measurement meas = {
            materia::Id::Invalid, 
            "meas_value_inside", 
            materia::Id("some icon id"), 
            0};

         meas.id = mService.addMeasurement(meas);
         mMeasurements.push_back(meas);
      }
      
      {
         materia::Goal g = createGoal(1);
         g.id = mService.addGoal(g);
         mGoals.push_back(g);

         mTasksOfGoals.insert(std::make_pair(g.id, createTestTasks(2, g.id)));
         mObjectivesOfGoals.insert(std::make_pair(g.id, createTestObjectives(3, g.id)));
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

         mTasksOfGoals.insert(std::make_pair(g.id, createTestTasks(7, g.id)));
      }
      {
         materia::Goal g = createGoal(4, mGoals[0].id);
         g.id = mService.addGoal(g);
         mGoals.push_back(g);

         mObjectivesOfGoals.insert(std::make_pair(g.id, createTestObjectives(9, g.id)));
      }

      for(int i = 0; i < 3; ++i)
      {
         materia::Affinity aff = {materia::Id::Invalid, "aff" + boost::lexical_cast<std::string>(i), materia::Id::Invalid, "someColor"};   
         mAffinities.push_back(aff);
      }
      mService.configureAffinities(mAffinities);
   }

protected:

   std::vector<materia::Task> createTestTasks(const int numTasks, const materia::Id& goalId)
   {
      std::vector<materia::Task> tasks;

      for(int i = 0; i < numTasks; ++i)
      {
         materia::Task t = createTask(i, goalId);
         if(i != 0)
         {
            t.requiredTasks.push_back(tasks[0].id);
         }
         t.id = mService.addTask(t);
         tasks.push_back(t);
      }

      return tasks;
   }

   std::vector<materia::Objective> createTestObjectives(const int num, const materia::Id& goalId)
   {
      std::vector<materia::Objective> result;

      for(int i = 0; i < num; ++i)
      {
         auto o = createObjective(i, goalId);
         o.id = mService.addObjective(o);
         result.push_back(o);
      }

      return result;
   }

   materia::Objective createObjective(const int suffix, const materia::Id& parentGoalId = materia::Id::Invalid)
   {
      materia::Objective o;

      o.reached = false;
      o.measurementId = mMeasurements[suffix % 2].id;
      o.name = "objective" + boost::lexical_cast<std::string>(suffix);
      o.notes = "notes of objective";
      o.parentGoalId = parentGoalId;
      o.iconId = materia::Id("some-icon-id");

      return o;
   }

   std::optional<materia::Goal> getGoal(const materia::Id& id)
   {
      auto goals = mService.getGoals();
      auto iter = std::find_if(goals.begin(), goals.end(), [&](auto x)->bool {return id == x.id;});
      if(iter != goals.end())
      {
         return *iter;
      }

      return std::optional<materia::Goal>();
   }

   void cleanUp()
   {
      mClient.getContainer().deleteContainer("goals");
      mClient.getContainer().deleteContainer("measurements");
      mClient.getContainer().deleteContainer("affinities");
   }

   bool hasEvent(const materia::EventType evType, const materia::Id& id)
   {
      struct : public materia::IEventHandler
      {
         virtual void onGenericEvent(const materia::Event& event)
         {
            
         }
         virtual void onContainerUpdated(const materia::ContainerUpdatedEvent& event)
         {
            
         }
         virtual void onIdEvent(const materia::IdEvent& event)
         {
            //To be added to events service
            found = mId == event.id;
         }

         materia::Id mId;
         bool found = false;
      } evHdr;

      evHdr.mId = id;
      mClient.getEvents().getEvents(boost::posix_time::from_time_t(0), evHdr);
      return evHdr.found;
   }

   materia::MateriaClient mClient;
   materia::Strategy& mService;
   std::vector<materia::Goal> mGoals;
   std::map<materia::Id, std::vector<materia::Task>> mTasksOfGoals;
   std::map<materia::Id, std::vector<materia::Objective>> mObjectivesOfGoals;
   std::vector<materia::Measurement> mMeasurements;
   std::vector<materia::Affinity> mAffinities;
};

BOOST_FIXTURE_TEST_CASE( ModifyGoal_Unchangable_Id, StrategyTest )  
{
   auto g = mGoals[0];
   g.id = materia::Id("other");
   BOOST_CHECK(!mService.modifyGoal(g));
}

BOOST_FIXTURE_TEST_CASE( ModifyGoal_Success, StrategyTest )  
{
   auto g = mGoals[0];
   g.parentGoalId = materia::Id::Invalid;
   g.name = "other_name";
   g.notes = "other_notes";
   g.iconId = materia::Id("other_id");
   g.requiredGoals.push_back(mGoals[1].id);
   g.focused = false;
   g.affinityId = materia::Id("affinity_id");

   BOOST_CHECK(mService.modifyGoal(g));
   BOOST_CHECK_EQUAL(g, *getGoal(g.id));
}

BOOST_FIXTURE_TEST_CASE( ModifyGoal_Req_Id_Must_Not_Be_Subgoal_Id, StrategyTest )  
{
   auto g = mGoals[0];
   g.requiredGoals.push_back(mGoals[2].id);
   BOOST_CHECK(!mService.modifyGoal(g));
}

BOOST_FIXTURE_TEST_CASE( ModifyGoal_Achieved_Is_Read_Only, StrategyTest )  
{
   auto g = mGoals[0];
   g.achieved = true;
   BOOST_CHECK(!mService.modifyGoal(g));
}

BOOST_FIXTURE_TEST_CASE( ModifyGoal_Unfocus_Modifies_Subgoals, StrategyTest )  
{
   auto g = mGoals[0];
   g.focused = false;
   BOOST_CHECK(mService.modifyGoal(g));
   BOOST_CHECK(!getGoal(mGoals[2].id)->focused);
   BOOST_CHECK(!getGoal(mGoals[3].id)->focused);

   BOOST_CHECK(hasEvent(materia::EventType::GoalUpdated, mGoals[0].id));
   BOOST_CHECK(hasEvent(materia::EventType::GoalUpdated, mGoals[2].id));
   BOOST_CHECK(hasEvent(materia::EventType::GoalUpdated, mGoals[3].id));
}

BOOST_FIXTURE_TEST_CASE( ModifyGoal_Change_Parent_To_Unfocused_Makes_Unfocused, StrategyTest )  
{
   auto g = mGoals[1];
   g.focused = false;
   BOOST_CHECK(mService.modifyGoal(g));

   g = mGoals[0];
   g.focused = true;
   g.parentGoalId = mGoals[1].id;
   BOOST_CHECK(mService.modifyGoal(g));

   BOOST_CHECK(!getGoal(g.id)->focused);
}

BOOST_FIXTURE_TEST_CASE( ModifyGoal_Cannot_Set_Children_As_Parent, StrategyTest )  
{
   auto g = mGoals[0];
   g.parentGoalId = g.id;
   BOOST_CHECK(!mService.modifyGoal(g));

   g.parentGoalId = mGoals[2].id;
   BOOST_CHECK(!mService.modifyGoal(g));
}

BOOST_FIXTURE_TEST_CASE( ModifyGoal_Check_Reqs_Are_Not_Cyclyc, StrategyTest )  
{
   auto g = mGoals[0];
   g.requiredGoals.push_back(mGoals[1].id);
   BOOST_CHECK(mService.modifyGoal(g));

   g = mGoals[1];
   g.requiredGoals.push_back(mGoals[0].id);
   BOOST_CHECK(!mService.modifyGoal(g));
}

BOOST_FIXTURE_TEST_CASE( ModifyGoal_Raise_Event, StrategyTest )  
{
   auto g = mGoals[1];
   g.focused = false;
   BOOST_CHECK(mService.modifyGoal(g));

   BOOST_CHECK(hasEvent(materia::EventType::GoalUpdated, mGoals[0].id));
}

BOOST_FIXTURE_TEST_CASE( DeleteGoal, StrategyTest )  
{
   //delete all subgoals, tasks, objectives
   //check goal_changed_event
}

BOOST_FIXTURE_TEST_CASE( GetGoals, StrategyTest )  
{
   //delete all subgoals, tasks, objectives
}

BOOST_FIXTURE_TEST_CASE( AddGoal, StrategyTest )  
{
   //invalid parent
   //invalid prereq
   //valid case
   //check req id is not cyclyc
   //check goal_changed_event
}

BOOST_FIXTURE_TEST_CASE( AddObjective, StrategyTest )  
{
   //invalid parent
   //invalid prereq
   //valid case
   //check goal_changed_event
   //check parent_goal.achieved changed based on all objective.reached
}

BOOST_FIXTURE_TEST_CASE( ModifyObjective, StrategyTest )  
{
   //invalid parent
   //valid case
   //check goal_changed_event
   //check reached changed if measurement/expected value is changed
   //check parent_goal.achieved changed based on all objective.reached
   //goals react to parent change
}

BOOST_FIXTURE_TEST_CASE( DeleteObjective, StrategyTest )  
{
   //valid case
   //check goal_changed_event
   //check parent_goal.achieved changed based on all objective.reached
}

BOOST_FIXTURE_TEST_CASE( AddTask, StrategyTest )  
{
   //invalid parent
   //invalid prereq
   //valid case
   //check goal_changed_event
   //check req id is not cyclyc
}

BOOST_FIXTURE_TEST_CASE( ModifyTask, StrategyTest )  
{
   //invalid parent
   //invalid prereq
   //valid case
   //check goal_changed_event
   //goals react to parent change
}

BOOST_FIXTURE_TEST_CASE( DeleteTask, StrategyTest )  
{
   //valid case
   //check goal_changed_event
}

BOOST_FIXTURE_TEST_CASE( GetGoalItems, StrategyTest )  
{
   
}

BOOST_FIXTURE_TEST_CASE( AddMeasurement, StrategyTest )  
{
   //check measurement_changed_event
}

BOOST_FIXTURE_TEST_CASE( ModifyMeasurement, StrategyTest )  
{
   //valid case
   //Check objectives -> goals reaction
   //check measurement_changed_event
}

BOOST_FIXTURE_TEST_CASE( DeleteMeasurement, StrategyTest )  
{
   //valid case
   //Check objectives -> goals reaction
   //check measurement_changed_event
}

BOOST_FIXTURE_TEST_CASE( GetMeasurements, StrategyTest )  
{
   //valid case
}

BOOST_FIXTURE_TEST_CASE( GetAffinities, StrategyTest )  
{
   //valid case
}

BOOST_FIXTURE_TEST_CASE( ConfigureAffinities, StrategyTest )  
{
   //valid case
   //check affinities_changed_event
}