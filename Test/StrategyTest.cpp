#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <Core/ICore.hpp>
#include <Core/IStrategy.hpp>

namespace std
{
   std::ostream& operator << (std::ostream& str, const materia::Goal& g)
   {
      str << "[" << g.id << ", "
         << g.name << ", " << g.notes << ", " << g.focused << ", " << g.achieved  << "]";
      return str;
   }

   std::ostream& operator << (std::ostream& str, const materia::Task& t)
   {
      str << "[" << t.id << ", " << t.parentGoalId << ", " 
         << t.name << ", " << t.notes << ", " << t.done << "]";
      return str;
   }

   std::ostream& operator << (std::ostream& str, const materia::Objective& o)
   {
      str << "[" << o.id << ", " << o.parentGoalId << ", " 
         << o.name << ", " << o.notes << ", " << o.reached << ", " << o.resourceId <<
         ", " << o.expectedResourceValue << "]";
      return str;
   }

   std::ostream& operator << (std::ostream& str, const materia::Resource& m)
   {
      str << "[" << m.id << ", " << m.value << ", " 
         << m.name << "]";
      return str;
   }
}

materia::Goal createGoal(const int suffix)
{
   materia::Goal g;
   g.name = "goal" + boost::lexical_cast<std::string>(suffix);
   g.notes = "notes of goal";
   g.focused = true;
   g.achieved = false;

   return g;
}

materia::Task createTask(const int suffix, const materia::Id& parentGoalId = materia::Id::Invalid)
{
   materia::Task t;
   t.done = true;
   t.name = "task" + boost::lexical_cast<std::string>(suffix);
   t.notes = "notes of task";
   t.parentGoalId = parentGoalId;

   return t;
}

class StrategyTest
{
public:
   StrategyTest()
   {
      system("rm Test.db");
      mCore = materia::createCore({"Test.db"});
      mStrategy = &mCore->getStrategy();

      //Test data - 4 gls (2, 3, 2 subgoals)(0, 0)(7, 0)(0, 9), 3 affinities, 3 resources (value, func)
      {
         materia::Resource res = {
            materia::Id::Invalid, 
            "res_value_inside", 
            0};

         res.id = mStrategy->addResource(res);
         mResources.push_back(res);
      }
      
      {
         materia::Goal g = createGoal(1);
         g.id = mStrategy->addGoal(g);
         mGoals.push_back(g);

         mTasksOfGoals.insert(std::make_pair(g.id, createTestTasks(2, g.id)));
         mObjectivesOfGoals.insert(std::make_pair(g.id, createTestObjectives(1, g.id)));
      }
      {
         materia::Goal g = createGoal(2);
         g.id = mStrategy->addGoal(g);
         mGoals.push_back(g);
      }
      {
         materia::Goal g = createGoal(3);
         g.id = mStrategy->addGoal(g);
         mGoals.push_back(g);

         mTasksOfGoals.insert(std::make_pair(g.id, createTestTasks(7, g.id)));
      }
      {
         materia::Goal g = createGoal(4);
         g.id = mStrategy->addGoal(g);
         mGoals.push_back(g);

         mObjectivesOfGoals.insert(std::make_pair(g.id, createTestObjectives(9, g.id)));
      }
   }

protected:

   std::vector<materia::Task> createTestTasks(const int numTasks, const materia::Id& goalId)
   {
      std::vector<materia::Task> tasks;

      for(int i = 0; i < numTasks; ++i)
      {
         materia::Task t = createTask(i, goalId);
         t.id = mStrategy->addTask(t);
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
         o.id = mStrategy->addObjective(o);
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

      return o;
   }

   std::optional<materia::Goal> getGoal(const materia::Id& id)
   {
      return mStrategy->getGoal(id);
   }

   std::optional<materia::Objective> findObjectiveInGoalItems(const materia::Id& objId, const materia::Id& goalId)
   {
      auto objs = std::get<1>(mStrategy->getGoalItems(goalId));
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
      auto tasks = std::get<0>(mStrategy->getGoalItems(goalId));
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

   std::optional<materia::Resource> getResource(const materia::Id& id)
   {
      auto reses = mStrategy->getResources();
      auto iter = materia::find_by_id(reses, id);

      if(iter != reses.end())
      {
         return *iter;
      }
      else
      {
         return std::optional<materia::Resource>();
      }
   }

   std::shared_ptr<materia::ICore> mCore;
   materia::IStrategy* mStrategy;

   std::vector<materia::Goal> mGoals;
   std::map<materia::Id, std::vector<materia::Task>> mTasksOfGoals;
   std::map<materia::Id, std::vector<materia::Objective>> mObjectivesOfGoals;
   std::vector<materia::Resource> mResources;
};

BOOST_FIXTURE_TEST_CASE( ModifyGoal_Unchangable_Id, StrategyTest )  
{
   auto g = mGoals[0];
   g.id = materia::Id("other");
   mStrategy->modifyGoal(g);

   BOOST_CHECK(!mStrategy->getGoal(g.id));
}

BOOST_FIXTURE_TEST_CASE( ModifyGoal_Success, StrategyTest )  
{
   auto g = mGoals[0];
   g.name = "other_name";
   g.notes = "other_notes";
   g.focused = false;

   mStrategy->modifyGoal(g);
   BOOST_CHECK_EQUAL(g, *getGoal(g.id));
}

BOOST_FIXTURE_TEST_CASE( ModifyGoal_Achieved_Is_Read_Only, StrategyTest )  
{
   auto g = mGoals[0];
   g.achieved = true;
   mStrategy->modifyGoal(g);

   BOOST_CHECK(!getGoal(g.id)->achieved);
}

BOOST_FIXTURE_TEST_CASE( DeleteGoal, StrategyTest )  
{
   mStrategy->deleteGoal(mGoals[0].id);

   auto [tasks, obs] = mStrategy->getGoalItems(mGoals[0].id);
   BOOST_CHECK(obs.empty());
   BOOST_CHECK(tasks.empty()); 

   auto remainingGoals = mStrategy->getGoals();
   BOOST_CHECK_EQUAL(3, remainingGoals.size());
   BOOST_CHECK(std::find_if(remainingGoals.begin(), remainingGoals.end(), [&](auto g)->bool{return g.id == mGoals[0].id;})
      == remainingGoals.end());
}

BOOST_FIXTURE_TEST_CASE( GetGoals, StrategyTest )  
{
   auto goals = mStrategy->getGoals();

   auto sortCriteria = [](auto a, auto b) -> bool
   { 
      return a.id < b.id; 
   };

   std::sort(goals.begin(), goals.end(), sortCriteria);
   std::sort(mGoals.begin(), mGoals.end(), sortCriteria);

   BOOST_CHECK_EQUAL_COLLECTIONS(goals.begin(), goals.end(), 
      mGoals.begin(), mGoals.end());
}

BOOST_FIXTURE_TEST_CASE( AddGoal, StrategyTest )  
{
   auto id = mStrategy->addGoal(materia::Goal{});
   BOOST_CHECK(materia::Id::Invalid != id);

   BOOST_CHECK_EQUAL(5, mStrategy->getGoals().size());
}

BOOST_FIXTURE_TEST_CASE( AddObjective_Invalid_Parent, StrategyTest )
{
   materia::Objective o;
   BOOST_CHECK_EQUAL(materia::Id::Invalid, mStrategy->addObjective(o));
}

BOOST_FIXTURE_TEST_CASE( AddObjective, StrategyTest )  
{
   materia::Objective o;
   o.parentGoalId = mGoals[0].id;
   o.id = mStrategy->addObjective(o);

   BOOST_CHECK(materia::Id::Invalid != o.id);
   BOOST_CHECK(findObjectiveInGoalItems(o.id, mGoals[0].id));
}

BOOST_FIXTURE_TEST_CASE( AddObjective_Goal_Achieved_Reaction, StrategyTest )  
{
   auto id = mStrategy->addGoal(materia::Goal{});
   auto g = getGoal(id);

   BOOST_CHECK(g);
   BOOST_CHECK(!g->achieved);

   materia::Objective o;
   o.parentGoalId = id;
   o.reached = true;

   mStrategy->addObjective(o);

   g = getGoal(id);

   BOOST_CHECK(g);
   BOOST_CHECK(g->achieved);
}

BOOST_FIXTURE_TEST_CASE( ModifyObjective_Invalid_Parent, StrategyTest )
{
   materia::Objective o = mObjectivesOfGoals[mGoals[0].id][0];
   o.parentGoalId = materia::Id::Invalid;
   mStrategy->modifyObjective(o);

   BOOST_CHECK(findObjectiveInGoalItems(o.id, mGoals[0].id));
}

BOOST_FIXTURE_TEST_CASE( ModifyObjective, StrategyTest )  
{
   const std::string newNotes = "sdkfasdjkf";
   materia::Objective o = mObjectivesOfGoals[mGoals[0].id][0];
   o.notes = newNotes;
   mStrategy->modifyObjective(o);

   BOOST_CHECK(findObjectiveInGoalItems(o.id, mGoals[0].id)->notes == newNotes);
}

BOOST_FIXTURE_TEST_CASE( ModifyObjective_Resources_Already_Achieved, StrategyTest )  
{
   materia::Objective o = mObjectivesOfGoals[mGoals[0].id][0];
   mResources[0].value = 5;
   mStrategy->modifyResource(mResources[0]);

   o.resourceId = mResources[0].id;
   o.expectedResourceValue = 4;

   mStrategy->modifyObjective(o);

   auto g = *getGoal(mGoals[0].id);
   BOOST_CHECK(g.achieved);

   BOOST_CHECK(findObjectiveInGoalItems(o.id, g.id)->reached);
}

BOOST_FIXTURE_TEST_CASE( ModifyObjective_Resources_Achieved_After_Change, StrategyTest )  
{
   materia::Objective o = mObjectivesOfGoals[mGoals[0].id][0];
   
   o.resourceId = mResources[0].id;
   o.expectedResourceValue = 4;

   mStrategy->modifyObjective(o);

   auto g = *getGoal(mGoals[0].id);
   BOOST_CHECK(!g.achieved);

   BOOST_CHECK(!findObjectiveInGoalItems(o.id, g.id)->reached);

   mResources[0].value = 5;
   mStrategy->modifyResource(mResources[0]);

   g = *getGoal(mGoals[0].id);
   BOOST_CHECK(g.achieved);

   BOOST_CHECK(findObjectiveInGoalItems(o.id, g.id)->reached);
}

BOOST_FIXTURE_TEST_CASE( ModifyObjective_Resources_NotAchieved_After_Change, StrategyTest )  
{
   materia::Objective o = mObjectivesOfGoals[mGoals[0].id][0];
   
   o.resourceId = mResources[0].id;
   o.expectedResourceValue = 4;

   mStrategy->modifyObjective(o);

   auto g = *getGoal(mGoals[0].id);
   BOOST_CHECK(!g.achieved);

   BOOST_CHECK(!findObjectiveInGoalItems(o.id, g.id)->reached);

   mResources[0].value = 3;
   mStrategy->modifyResource(mResources[0]);

   g = *getGoal(g.id);
   BOOST_CHECK(!g.achieved);

   BOOST_CHECK(!findObjectiveInGoalItems(o.id, g.id)->reached);

   
}

BOOST_FIXTURE_TEST_CASE( ModifyObjective_n_minus_1_objectives_reached, StrategyTest )  
{
   auto g = *getGoal(mGoals[3].id);
   BOOST_CHECK(!g.achieved);

   for(auto o : mObjectivesOfGoals[g.id])
   {
      g = *getGoal(g.id);
      BOOST_CHECK(!g.achieved);

      o.reached = true;

      mStrategy->modifyObjective(o);
   }

   g = *getGoal(g.id);
   BOOST_CHECK(g.achieved);
}

BOOST_FIXTURE_TEST_CASE( ModifyObjective_objective_parent_change, StrategyTest )  
{
   materia::Objective o = mObjectivesOfGoals[mGoals[0].id][0];
   
   o.parentGoalId = mGoals[1].id;

   mStrategy->modifyObjective(o);

   BOOST_CHECK(!findObjectiveInGoalItems(o.id, mGoals[0].id));
   BOOST_CHECK(findObjectiveInGoalItems(o.id, mGoals[1].id));
}

BOOST_FIXTURE_TEST_CASE( DeleteObjective, StrategyTest )  
{
   auto id = mObjectivesOfGoals[mGoals[0].id][0].id;
   mStrategy->deleteObjective(id);
   BOOST_CHECK(!findObjectiveInGoalItems(id, mGoals[0].id));
}

BOOST_FIXTURE_TEST_CASE( AddTask, StrategyTest )  
{
   materia::Task t;
   t.parentGoalId = mGoals[0].id;
   auto id = mStrategy->addTask(t);
   BOOST_CHECK(id != materia::Id::Invalid);
   BOOST_CHECK(findTaskInGoalItems(id, mGoals[0].id));
}

BOOST_FIXTURE_TEST_CASE( ModifyTask_invalid_parent, StrategyTest )  
{
   auto t = mTasksOfGoals[mGoals[0].id][0];
   t.parentGoalId = materia::Id::Invalid;
   mStrategy->modifyTask(t);

   BOOST_CHECK(!findTaskInGoalItems(t.id, mGoals[0].id));
}

BOOST_FIXTURE_TEST_CASE( ModifyTask, StrategyTest )  
{
   auto newName = "toher";
   auto newNotes = "other";
   auto t = mTasksOfGoals[mGoals[0].id][0];
   t.name = newName;
   t.notes = newNotes;
   t.done = true;

   mStrategy->modifyTask(t);

   auto newTask = findTaskInGoalItems(t.id, mGoals[0].id);
   BOOST_CHECK(newTask);

   BOOST_CHECK_EQUAL(newTask->name, newName);
   BOOST_CHECK_EQUAL(newTask->notes, newNotes);
   BOOST_CHECK(newTask->done);
}

BOOST_FIXTURE_TEST_CASE( ModifyTask_reparent, StrategyTest )  
{
   materia::Task t = mTasksOfGoals[mGoals[0].id][0];
   
   t.parentGoalId = mGoals[1].id;

   mStrategy->modifyTask(t);

   BOOST_CHECK(!findTaskInGoalItems(t.id, mGoals[0].id));
   BOOST_CHECK(findTaskInGoalItems(t.id, mGoals[1].id));
}

BOOST_FIXTURE_TEST_CASE( DeleteTask, StrategyTest )  
{
   materia::Task t = mTasksOfGoals[mGoals[0].id][0];

   auto tasks = std::get<0>(mStrategy->getGoalItems(mGoals[0].id));

   mStrategy->deleteTask(t.id);
   BOOST_CHECK(!findTaskInGoalItems(t.id, mGoals[0].id));
}

BOOST_FIXTURE_TEST_CASE( GetGoalItems, StrategyTest )  
{
   for(auto x : mGoals)
   {
      auto goalItems = mStrategy->getGoalItems(x.id);

      auto sortCriteria = [](auto a, auto b) -> bool
      { 
         return a.id < b.id; 
      };

      auto tasksGot = std::get<0>(goalItems);
      std::sort(tasksGot.begin(), tasksGot.end(), sortCriteria);
      auto taskExpected = mTasksOfGoals[x.id];
      std::sort(taskExpected.begin(), taskExpected.end(), sortCriteria);

      BOOST_CHECK_EQUAL_COLLECTIONS(tasksGot.begin(), tasksGot.end(), 
         taskExpected.begin(), taskExpected.end());

      auto objectivesGot = std::get<1>(goalItems);
      std::sort(objectivesGot.begin(), objectivesGot.end(), sortCriteria);

      auto objectivesExpected = mObjectivesOfGoals[x.id];
      std::sort(objectivesExpected.begin(), objectivesExpected.end(), sortCriteria);

      BOOST_CHECK_EQUAL_COLLECTIONS(objectivesGot.begin(), objectivesGot.end(), 
         objectivesExpected.begin(), objectivesExpected.end());
   }
}

BOOST_FIXTURE_TEST_CASE( AddResource, StrategyTest )  
{
   materia::Resource res = {
      materia::Id::Invalid, 
      "res_value_inside",
      0};

   res.id = mStrategy->addResource(res);
   BOOST_CHECK(materia::Id::Invalid != res.id);
}

BOOST_FIXTURE_TEST_CASE( ModifyResource, StrategyTest )  
{
   auto newName = "other_name";
   auto newValue = 54;

   auto res = mResources[0];
   res.name = newName;
   res.value = newValue;

   mStrategy->modifyResource(res);

   auto m = getResource(res.id);
   BOOST_CHECK(m);

   BOOST_CHECK_EQUAL(m->name, newName);
   BOOST_CHECK_EQUAL(m->value, newValue);
}

BOOST_FIXTURE_TEST_CASE( DeleteResource, StrategyTest )  
{
   mStrategy->deleteResource(mResources[0].id);
   BOOST_CHECK(!getResource(mResources[0].id));
}

BOOST_FIXTURE_TEST_CASE( GetResources, StrategyTest )  
{
   auto expected = mResources;
   auto got = mStrategy->getResources();

   BOOST_CHECK_EQUAL_COLLECTIONS(got.begin(), got.end(), 
         expected.begin(), expected.end());
}

BOOST_FIXTURE_TEST_CASE( EmptyGoalNotAchieved, StrategyTest )  
{
   materia::Goal g;
   g.achieved = true;

   g.id = mStrategy->addGoal(g);

   g = *getGoal(g.id);

   BOOST_CHECK(!g.achieved);
}