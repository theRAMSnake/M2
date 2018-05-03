#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <Client/MateriaClient.hpp>
#include <Client/Strategy.hpp>
#include "TestEventListener.hpp"
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

   std::ostream& operator << (std::ostream& str, const materia::Task& t)
   {
      str << "[" << t.id << ", " << t.parentGoalId << ", " 
         << t.name << ", " << t.notes << ", " << t.iconId << ", " << t.done << ", " << t.requiredTasks.size() << "]";
      return str;
   }

   std::ostream& operator << (std::ostream& str, const materia::Objective& o)
   {
      str << "[" << o.id << ", " << o.parentGoalId << ", " 
         << o.name << ", " << o.notes << ", " << o.iconId << ", " << o.reached << ", " << o.measurementId <<
         ", " << o.expected << "]";
      return str;
   }

   std::ostream& operator << (std::ostream& str, const materia::Measurement& m)
   {
      str << "[" << m.id << ", " << m.value << ", " 
         << m.name << ", " << m.iconId << "]";
      return str;
   }

   std::ostream& operator << (std::ostream& str, const materia::Affinity& a)
   {
      str << "[" << a.id << ", " << a.colorName << ", " 
         << a.name << ", " << a.iconId << "]";
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
   , mEventListener(mClient.getEvents())
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
         mObjectivesOfGoals.insert(std::make_pair(g.id, createTestObjectives(1, g.id)));
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
      o.name = "objective" + boost::lexical_cast<std::string>(suffix);
      o.notes = "notes of objective";
      o.parentGoalId = parentGoalId;
      o.iconId = materia::Id("some-icon-id");

      return o;
   }

   std::optional<materia::Goal> getGoal(const materia::Id& id)
   {
      return mService.getGoal(id);
   }

   void cleanUp()
   {
      mClient.getStrategy().clear();
   }

   std::optional<materia::Objective> findObjectiveInGoalItems(const materia::Id& objId, const materia::Id& goalId)
   {
      auto objs = std::get<1>(mService.getGoalItems(goalId));
      auto iter = std::find_if(objs.begin(), objs.end(), [&] (auto x)->bool {return x.id == objId;});
      if(iter != objs.end())
      {
         return *iter;
      }
      else
      {
         return std::optional<materia::Objective>();
      }
   }

   std::optional<materia::Task> findTaskInGoalItems(const materia::Id& taskId, const materia::Id& goalId)
   {
      auto tasks = std::get<0>(mService.getGoalItems(goalId));
      auto iter = std::find_if(tasks.begin(), tasks.end(), [&] (auto x)->bool {return x.id == taskId;});
      if(iter != tasks.end())
      {
         return *iter;
      }
      else
      {
         return std::optional<materia::Task>();
      }
   }

   materia::MateriaClient mClient;
   TestEventListener mEventListener;
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

   BOOST_CHECK(mEventListener.hasEvent<materia::EventType::GoalUpdated>(mGoals[0].id));
   BOOST_CHECK(mEventListener.hasEvent<materia::EventType::GoalUpdated>(mGoals[2].id));
   BOOST_CHECK(mEventListener.hasEvent<materia::EventType::GoalUpdated>(mGoals[3].id));
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

   BOOST_CHECK(mEventListener.hasEvent<materia::EventType::GoalUpdated>(mGoals[0].id));
}

BOOST_FIXTURE_TEST_CASE( DeleteGoal, StrategyTest )  
{
   BOOST_CHECK(mService.deleteGoal(mGoals[0].id));

   BOOST_CHECK(std::get<0>(mService.getGoalItems(mGoals[0].id)).empty());
   BOOST_CHECK(std::get<1>(mService.getGoalItems(mGoals[0].id)).empty());
   auto remainingGoals = mService.getGoals();
   BOOST_CHECK_EQUAL(1, remainingGoals.size());
   BOOST_CHECK_EQUAL(remainingGoals[0].id, mGoals[1].id);

   BOOST_CHECK(mEventListener.hasEvent<materia::EventType::GoalUpdated>(mGoals[0].id));
}

BOOST_FIXTURE_TEST_CASE( GetGoals, StrategyTest )  
{
   auto goals = mService.getGoals();

   BOOST_CHECK_EQUAL_COLLECTIONS(goals.begin(), goals.end(), 
      mGoals.begin(), mGoals.end());
}

BOOST_FIXTURE_TEST_CASE( AddGoal_Invalid_Parent, StrategyTest )  
{
   materia::Goal g = {materia::Id::Invalid, materia::Id("sadfjhsjkd")};
   BOOST_CHECK_EQUAL(materia::Id::Invalid, mService.addGoal(g));

   BOOST_CHECK_EQUAL(4, mService.getGoals().size());
}

BOOST_FIXTURE_TEST_CASE( AddGoal, StrategyTest )  
{
   auto id = mService.addGoal(materia::Goal{});
   BOOST_CHECK(materia::Id::Invalid != id);

   BOOST_CHECK_EQUAL(5, mService.getGoals().size());

   BOOST_CHECK(mEventListener.hasEvent<materia::EventType::GoalUpdated>(id));
}

BOOST_FIXTURE_TEST_CASE( AddObjective_Invalid_Parent, StrategyTest )
{
   materia::Objective o = {materia::Id::Invalid, materia::Id::Invalid};
   BOOST_CHECK_EQUAL(materia::Id::Invalid, mService.addObjective(o));
}

BOOST_FIXTURE_TEST_CASE( AddObjective, StrategyTest )  
{
   materia::Objective o = {materia::Id::Invalid, mGoals[0].id};
   auto id = mService.addObjective(o);
   BOOST_CHECK(materia::Id::Invalid != id);

   BOOST_CHECK(mEventListener.hasEvent<materia::EventType::GoalUpdated>(mGoals[0].id));

   BOOST_CHECK(findObjectiveInGoalItems(mGoals[0].id, o.id));
}

BOOST_FIXTURE_TEST_CASE( AddObjective_Goal_Achieved_Reaction, StrategyTest )  
{
   auto id = mService.addGoal(materia::Goal{});
   auto g = getGoal(id);

   BOOST_CHECK(g);
   BOOST_CHECK(g->achieved);

   materia::Objective o;
   o.parentGoalId = id;
   o.reached = false;

   mService.addObjective(o);

   g = getGoal(id);

   BOOST_CHECK(g);
   BOOST_CHECK(!g->achieved);
}

BOOST_FIXTURE_TEST_CASE( ModifyObjective_Invalid_Parent, StrategyTest )
{
   materia::Objective o = mObjectivesOfGoals[mGoals[0].id][0];
   o.parentGoalId = materia::Id::Invalid;
   BOOST_CHECK(!mService.modifyObjective(o));
}

BOOST_FIXTURE_TEST_CASE( ModifyObjective, StrategyTest )  
{
   materia::Objective o = mObjectivesOfGoals[mGoals[0].id][0];
   o.notes = "sdkfasdjkf";
   BOOST_CHECK(mService.modifyObjective(o));

   BOOST_CHECK(mEventListener.hasEvent<materia::EventType::GoalUpdated>(mGoals[0].id));
}

BOOST_FIXTURE_TEST_CASE( ModifyObjective_Measurements_Already_Achieved, StrategyTest )  
{
   materia::Objective o = mObjectivesOfGoals[mGoals[0].id][0];
   mMeasurements[0].value = 5;
   mService.modifyMeasurement(mMeasurements[0]);

   o.measurementId = mMeasurements[0].id;
   o.expected = 4;

   mService.modifyObjective(o);

   BOOST_CHECK(mEventListener.hasEvent<materia::EventType::GoalUpdated>(mGoals[0].id));
   auto g = *getGoal(mGoals[0].id);
   BOOST_CHECK(g.achieved);

   BOOST_CHECK(findObjectiveInGoalItems(o.id, g.id)->reached);
}

BOOST_FIXTURE_TEST_CASE( ModifyObjective_Measurements_Achieved_After_Change, StrategyTest )  
{
   materia::Objective o = mObjectivesOfGoals[mGoals[0].id][0];
   
   o.measurementId = mMeasurements[0].id;
   o.expected = 4;

   mService.modifyObjective(o);

   auto g = *getGoal(mGoals[0].id);
   BOOST_CHECK(!g.achieved);

   BOOST_CHECK(!findObjectiveInGoalItems(o.id, g.id)->reached);

   mMeasurements[0].value = 5;
   mService.modifyMeasurement(mMeasurements[0]);

   BOOST_CHECK(mEventListener.hasEvent<materia::EventType::GoalUpdated>(mGoals[0].id));
   g = *getGoal(mGoals[0].id);
   BOOST_CHECK(g.achieved);

   BOOST_CHECK(findObjectiveInGoalItems(o.id, g.id)->reached);
}

BOOST_FIXTURE_TEST_CASE( ModifyObjective_Measurements_NotAchieved_After_Change, StrategyTest )  
{
   materia::Objective o = mObjectivesOfGoals[mGoals[0].id][0];
   
   o.measurementId = mMeasurements[0].id;
   o.expected = 4;

   mService.modifyObjective(o);

   auto g = *getGoal(mGoals[0].id);
   BOOST_CHECK(!g.achieved);

   BOOST_CHECK(!findObjectiveInGoalItems(o.id, g.id)->reached);

   mMeasurements[0].value = 3;
   mService.modifyMeasurement(mMeasurements[0]);

   BOOST_CHECK(!hasEvent(materia::EventType::GoalUpdated, mGoals[0].id));
   g = *getGoal(g.id);
   BOOST_CHECK(!g.achieved);

   BOOST_CHECK(!findObjectiveInGoalItems(o.id, g.id)->reached);
}

BOOST_FIXTURE_TEST_CASE( ModifyObjective_n_minus_1_objectives_reached, StrategyTest )  
{
   auto g = *getGoal(mGoals[2].id);
   BOOST_CHECK(!g.achieved);

   for(auto o : mObjectivesOfGoals[g.id])
   {
      g = *getGoal(g.id);
      BOOST_CHECK(!g.achieved);

      o.reached = true;

      mService.modifyObjective(o);
   }

   g = *getGoal(g.id);
   BOOST_CHECK(g.achieved);
}

BOOST_FIXTURE_TEST_CASE( ModifyObjective_objective_parent_change, StrategyTest )  
{
   materia::Objective o = mObjectivesOfGoals[mGoals[0].id][0];
   
   o.parentGoalId = mGoals[1].id;

   mService.modifyObjective(o);

   BOOST_CHECK(mEventListener.hasEvent<materia::EventType::GoalUpdated>(mGoals[0].id));
   BOOST_CHECK(mEventListener.hasEvent<materia::EventType::GoalUpdated>(mGoals[1].id));

   BOOST_CHECK(!findObjectiveInGoalItems(o.id, mGoals[0].id));
   BOOST_CHECK(findObjectiveInGoalItems(o.id, mGoals[1].id));
}

BOOST_FIXTURE_TEST_CASE( DeleteObjective, StrategyTest )  
{
   auto id = mObjectivesOfGoals[mGoals[0].id][0].id;
   mService.deleteObjective(id);
   BOOST_CHECK(!findObjectiveInGoalItems(id, mGoals[0].id));
   BOOST_CHECK(mEventListener.hasEvent<materia::EventType::GoalUpdated>(mGoals[0].id));
}

BOOST_FIXTURE_TEST_CASE( AddTask, StrategyTest )  
{
   BOOST_CHECK_EQUAL(materia::Id::Invalid, mService.addTask(materia::Task{}));

   auto id = mService.addTask(materia::Task{materia::Id::Invalid, mGoals[0].id});
   BOOST_CHECK(id != materia::Id::Invalid);
   BOOST_CHECK(mEventListener.hasEvent<materia::EventType::GoalUpdated>(mGoals[0].id));
   BOOST_CHECK(findObjectiveInGoalItems(id, mGoals[0].id));
}

BOOST_FIXTURE_TEST_CASE( ModifyTask_invalid_parent, StrategyTest )  
{
   auto t = mTasksOfGoals[mGoals[0].id][0];
   t.parentGoalId = materia::Id::Invalid;
   BOOST_CHECK(!mService.modifyTask(t));
}

BOOST_FIXTURE_TEST_CASE( ModifyTask_invalid_prereq, StrategyTest )  
{
   auto t = mTasksOfGoals[mGoals[0].id][0];
   t.requiredTasks.push_back(t.id);
   BOOST_CHECK(!mService.modifyTask(t));

   t = mTasksOfGoals[mGoals[0].id][0];
   t.requiredTasks.push_back(mTasksOfGoals[mGoals[0].id][1].id);
   BOOST_CHECK(!mService.modifyTask(t));
}

BOOST_FIXTURE_TEST_CASE( ModifyTask, StrategyTest )  
{
   auto t = mTasksOfGoals[mGoals[0].id][0];
   t.name = "toher";
   t.notes = "other";
   t.iconId = materia::Id("other");
   t.done = true;

   BOOST_CHECK(mService.modifyTask(t));
   BOOST_CHECK(mEventListener.hasEvent<materia::EventType::GoalUpdated>(mGoals[0].id));
}

BOOST_FIXTURE_TEST_CASE( ModifyTask_reparent, StrategyTest )  
{
   materia::Task t = mTasksOfGoals[mGoals[0].id][0];
   
   t.parentGoalId = mGoals[1].id;

   BOOST_CHECK(mService.modifyTask(t));

   BOOST_CHECK(mEventListener.hasEvent<materia::EventType::GoalUpdated>(mGoals[0].id));
   BOOST_CHECK(mEventListener.hasEvent<materia::EventType::GoalUpdated>(mGoals[1].id));

   BOOST_CHECK(!findTaskInGoalItems(t.id, mGoals[0].id));
   BOOST_CHECK(findTaskInGoalItems(t.id, mGoals[1].id));
}

BOOST_FIXTURE_TEST_CASE( DeleteTask, StrategyTest )  
{
   materia::Task t = mTasksOfGoals[mGoals[0].id][0];
   BOOST_CHECK(mService.modifyTask(t));
   BOOST_CHECK(mEventListener.hasEvent<materia::EventType::GoalUpdated>(mGoals[0].id));
   BOOST_CHECK(!findTaskInGoalItems(t.id, mGoals[0].id));
}

BOOST_FIXTURE_TEST_CASE( GetGoalItems, StrategyTest )  
{
   for(auto x : mGoals)
   {
      auto goalItems = mService.getGoalItems(x.id);

      auto tasksGot = std::get<0>(goalItems);
      auto taskExpected = mTasksOfGoals[x.id];

      BOOST_CHECK_EQUAL_COLLECTIONS(tasksGot.begin(), tasksGot.end(), 
         taskExpected.begin(), taskExpected.end());

      auto objectivesGot = std::get<1>(goalItems);
      auto objectivesExpected = mObjectivesOfGoals[x.id];

      BOOST_CHECK_EQUAL_COLLECTIONS(objectivesGot.begin(), objectivesGot.end(), 
         objectivesExpected.begin(), objectivesExpected.end());
   }
}

BOOST_FIXTURE_TEST_CASE( AddMeasurement, StrategyTest )  
{
   materia::Measurement meas = {
      materia::Id::Invalid, 
      "meas_value_inside", 
      materia::Id("some icon id"), 
      0};

   meas.id = mService.addMeasurement(meas);
   BOOST_CHECK(materia::Id::Invalid != meas.id);

   BOOST_CHECK(mEventListener.hasEvent<materia::EventType::MeasurementUpdated>(meas.id));
}

BOOST_FIXTURE_TEST_CASE( ModifyMeasurement, StrategyTest )  
{
   auto meas = mMeasurements[0];
   meas.name = "other_name";
   meas.iconId = materia::Id("some_other_id");
   meas.value = 54;

   BOOST_CHECK(mService.modifyMeasurement(meas));
   BOOST_CHECK(mEventListener.hasEvent<materia::EventType::MeasurementUpdated>(meas.id));
}

BOOST_FIXTURE_TEST_CASE( DeleteMeasurement, StrategyTest )  
{
   BOOST_CHECK(mService.deleteMeasurement(mMeasurements[0].id));
   BOOST_CHECK(mEventListener.hasEvent<materia::EventType::MeasurementUpdated>(mMeasurements[0].id));
}

BOOST_FIXTURE_TEST_CASE( GetMeasurements, StrategyTest )  
{
   auto expected = mMeasurements;
   auto got = mService.getMeasurements();

   BOOST_CHECK_EQUAL_COLLECTIONS(got.begin(), got.end(), 
         expected.begin(), expected.end());
}

BOOST_FIXTURE_TEST_CASE( GetAffinities, StrategyTest )  
{
   auto expected = mAffinities;
   auto got = mService.getAffinities();

   BOOST_CHECK_EQUAL_COLLECTIONS(got.begin(), got.end(), 
         expected.begin(), expected.end());
}

BOOST_FIXTURE_TEST_CASE( ConfigureAffinities, StrategyTest )  
{
   for(auto x : mAffinities)
   {
      x.name += "ddd";
   }

   mService.configureAffinities(mAffinities);

   auto expected = mAffinities;
   auto got = mService.getAffinities();

   BOOST_CHECK_EQUAL_COLLECTIONS(got.begin(), got.end(), 
         expected.begin(), expected.end());
   
   BOOST_CHECK(mEventListener.hasEvent<materia::EventType::AffinitiesUpdated>());
}

BOOST_FIXTURE_TEST_CASE( EmptyGoalNotAchieved, StrategyTest )  
{
   materia::Goal g;
   g.achieved = true;

   g.id = mService.addGoal(g);

   g = *getGoal(g.id);

   BOOST_CHECK(!g.achieved);
}

BOOST_FIXTURE_TEST_CASE( Goal_Make_achieved_step_by_step, StrategyTest )  
{
   materia::Goal primaryGoal;
   primaryGoal.id = mService.addGoal(primaryGoal);

   materia::Goal secondaryGoalA;
   secondaryGoalA.parentGoalId = primaryGoal.id;
   secondaryGoalA.id = mService.addGoal(secondaryGoalA);

   materia::Goal secondaryGoalB;
   secondaryGoalB.parentGoalId = primaryGoal.id;
   secondaryGoalB.id = mService.addGoal(secondaryGoalB);

   materia::Objective oA1, oA2, oB1, oB2;

   oA1.parentGoalId = secondaryGoalA.id;
   oA1.reached = false;
   oA1.id = mService.addObjective(oA1);  

   oA2.parentGoalId = secondaryGoalA.id;
   oA2.reached = false;
   oA2.id = mService.addObjective(oA2);  

   oB1.parentGoalId = secondaryGoalB.id;
   oB1.reached = false;
   oB1.id = mService.addObjective(oB1);  

   oB2.parentGoalId = secondaryGoalB.id;
   oB2.reached = false;
   oB2.id = mService.addObjective(oB2);  

   BOOST_CHECK(!getGoal(primaryGoal.id)->achieved);
   BOOST_CHECK(!getGoal(secondaryGoalA.id)->achieved);
   BOOST_CHECK(!getGoal(secondaryGoalB.id)->achieved);

   oB1.reached = true;
   BOOST_CHECK(mService.modifyObjective(oB1));

   BOOST_CHECK(!getGoal(primaryGoal.id)->achieved);
   BOOST_CHECK(!getGoal(secondaryGoalA.id)->achieved);
   BOOST_CHECK(!getGoal(secondaryGoalB.id)->achieved);

   oB2.reached = true;
   BOOST_CHECK(mService.modifyObjective(oB2));

   BOOST_CHECK(!getGoal(primaryGoal.id)->achieved);
   BOOST_CHECK(!getGoal(secondaryGoalA.id)->achieved);
   BOOST_CHECK(getGoal(secondaryGoalB.id)->achieved);

   oA1.reached = true;
   BOOST_CHECK(mService.modifyObjective(oA1));

   BOOST_CHECK(!getGoal(primaryGoal.id)->achieved);
   BOOST_CHECK(!getGoal(secondaryGoalA.id)->achieved);
   BOOST_CHECK(getGoal(secondaryGoalB.id)->achieved);

   oA2.reached = true;
   BOOST_CHECK(mService.modifyObjective(oA2));

   BOOST_CHECK(getGoal(primaryGoal.id)->achieved);
   BOOST_CHECK(getGoal(secondaryGoalA.id)->achieved);
   BOOST_CHECK(getGoal(secondaryGoalB.id)->achieved);
}