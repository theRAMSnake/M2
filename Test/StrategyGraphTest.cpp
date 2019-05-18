#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <Core/ICore.hpp>
#include <Core/IStrategy_v2.hpp>

namespace std
{
   static std::ostream& operator << (std::ostream& str, const materia::Goal& g)
   {
      str << "[" << g.id << ", " << g.name << ", " << g.notes << ", " << g.focused << ", " << g.achieved  << "]";
      return str;
   }

   static std::ostream& operator << (std::ostream& str, const materia::NodeType& n)
   {
      switch(n)
      {
         case materia::NodeType::Goal:
            str << "Goal";
            break;

         case materia::NodeType::Task:
            str << "Task";
            break;

         case materia::NodeType::Blank:
            str << "Blank";
            break;

         default:
            str << n;
            break;
      }
      return str;
   }
}

static materia::Goal createGoal(const int suffix)
{
   materia::Goal g;
   g.name = "goal" + std::to_string(suffix);
   g.notes = "notes of goal";
   g.focused = true;
   g.achieved = false;

   return g;
}

std::shared_ptr<materia::ICore> createTestCore()
{
   system("rm Test.db");
   return materia::createCore({"Test.db"});
}

class StrategyGraphTest
{
public:
   StrategyGraphTest()
   : mCore(createTestCore())
   , mStrategy(mCore->getStrategy_v2())
   {
      for(int i = 0; i < 3; ++i)
      {
         materia::Goal g = createGoal(i);
         g.id = mStrategy.addGoal(g);
         mGoals.push_back(g);
      }
   }

protected:
   std::vector<materia::Goal> mGoals;

   std::shared_ptr<materia::ICore> mCore;
   materia::IStrategy_v2& mStrategy;
};

BOOST_FIXTURE_TEST_CASE( StrategyGraphTest_AddGoal, StrategyGraphTest )  
{
   auto id = mStrategy.addGoal(createGoal(8));

   BOOST_CHECK(mStrategy.getGoal(id));

   auto g = mStrategy.getGraph(id);
   BOOST_CHECK(g);

   BOOST_CHECK_EQUAL(1, g->getNodes().size());
   BOOST_CHECK_EQUAL(materia::NodeType::Goal, g->getNodes()[0].type);
}

BOOST_FIXTURE_TEST_CASE( StrategyGraphTest_ModifyGoal_Unchangable_Id, StrategyGraphTest )  
{
   auto g = mGoals[0];
   g.id = materia::Id("other");
   mStrategy.modifyGoal(g);

   BOOST_CHECK(!mStrategy.getGoal(g.id));
}

BOOST_FIXTURE_TEST_CASE( StrategyGraphTest_ModifyGoal_Success, StrategyGraphTest )  
{
   auto g = mGoals[0];
   g.name = "other_name";
   g.notes = "other_notes";
   g.focused = false;

   mStrategy.modifyGoal(g);
   BOOST_CHECK_EQUAL(g, *mStrategy.getGoal(g.id));
}

BOOST_FIXTURE_TEST_CASE( StrategyGraphTest_ModifyGoal_Achieved_Is_Read_Only, StrategyGraphTest )  
{
   auto g = mGoals[0];
   g.achieved = true;
   mStrategy.modifyGoal(g);

   BOOST_CHECK(!mStrategy.getGoal(g.id)->achieved);
}

BOOST_FIXTURE_TEST_CASE( StrategyGraphTest_DeleteGoal, StrategyGraphTest )  
{
   mStrategy.deleteGoal(mGoals[0].id);

   auto remainingGoals = mStrategy.getGoals();
   BOOST_CHECK_EQUAL(2, remainingGoals.size());
   BOOST_CHECK(std::find_if(remainingGoals.begin(), remainingGoals.end(), [&](auto g)->bool{return g.id == mGoals[0].id;})
      == remainingGoals.end());

   BOOST_CHECK(!mStrategy.getGraph(mGoals[0].id));
}

BOOST_FIXTURE_TEST_CASE( StrategyGraphTest_GetGoals, StrategyGraphTest )  
{
   auto goals = mStrategy.getGoals();

   auto sortCriteria = [](auto a, auto b) -> bool
   { 
      return a.id < b.id; 
   };

   std::sort(goals.begin(), goals.end(), sortCriteria);
   std::sort(mGoals.begin(), mGoals.end(), sortCriteria);

   BOOST_CHECK_EQUAL_COLLECTIONS(goals.begin(), goals.end(), 
      mGoals.begin(), mGoals.end());
}

BOOST_FIXTURE_TEST_CASE( StrategyGraphTest_GetGoal, StrategyGraphTest )  
{
   for(std::size_t i = 0; i < mGoals.size(); ++i)
   {
      auto g = mStrategy.getGoal(mGoals[i].id);
      BOOST_REQUIRE(g);
      BOOST_CHECK_EQUAL(mGoals[i], *g);
   }
}

BOOST_FIXTURE_TEST_CASE( StrategyGraphTest_CreateNode, StrategyGraphTest )  
{
   auto graphId = mGoals[0].id;
   auto nodeId = mStrategy.createNode(graphId);

   auto nodes = mStrategy.getGraph(graphId)->getNodes();
   BOOST_CHECK(materia::contains_id(nodes, nodeId));
}

BOOST_FIXTURE_TEST_CASE( StrategyGraphTest_CreateNodeInvalid, StrategyGraphTest )  
{
   auto nodeId = mStrategy.createNode(materia::Id("invalid"));

   BOOST_CHECK_EQUAL(materia::Id::Invalid, nodeId);
}

BOOST_FIXTURE_TEST_CASE( StrategyGraphTest_CreateLink, StrategyGraphTest )
{
   auto graphId = mGoals[0].id;
   auto nodeId1 = mStrategy.createNode(graphId);
   auto nodeId2 = mStrategy.createNode(graphId);

   mStrategy.createLink(graphId, nodeId1, nodeId2);

   auto g = mStrategy.getGraph(graphId);
   auto nodes = g->getNodes();
   auto links = g->getLinks();

   BOOST_CHECK(materia::contains_id(nodes, nodeId1));
   BOOST_CHECK(materia::contains_id(nodes, nodeId2));
   BOOST_CHECK_EQUAL(1, links.size());
   BOOST_CHECK_EQUAL(nodeId1, links[0].from);
   BOOST_CHECK_EQUAL(nodeId2, links[0].to);
}

BOOST_FIXTURE_TEST_CASE( StrategyGraphTest_CreateLinkWrongIds, StrategyGraphTest )
{
   auto graphId = mGoals[0].id;
   auto nodeId1 = materia::Id::Invalid;
   auto nodeId2 = mStrategy.createNode(graphId);
   auto g = mStrategy.getGraph(graphId);

   {
      mStrategy.createLink(graphId, nodeId1, nodeId2);
      BOOST_CHECK(g->getLinks().empty());
   }
   {
      mStrategy.createLink(graphId, nodeId2, nodeId1);
      BOOST_CHECK(g->getLinks().empty());
   }
   {
      mStrategy.createLink(graphId, nodeId1, nodeId1);
      BOOST_CHECK(g->getLinks().empty());
   }
}

BOOST_FIXTURE_TEST_CASE( StrategyGraphTest_CreateLinkAlreadyExist, StrategyGraphTest )
{
   auto graphId = mGoals[0].id;
   auto nodeId1 = mStrategy.createNode(graphId);
   auto nodeId2 = mStrategy.createNode(graphId);

   mStrategy.createLink(graphId, nodeId1, nodeId2);
   mStrategy.createLink(graphId, nodeId1, nodeId2);
   mStrategy.createLink(graphId, nodeId2, nodeId1);

   auto g = mStrategy.getGraph(graphId);
   auto nodes = g->getNodes();

   BOOST_CHECK(materia::contains_id(nodes, nodeId1));
   BOOST_CHECK(materia::contains_id(nodes, nodeId2));
   BOOST_CHECK_EQUAL(1, g->getLinks().size());
   BOOST_CHECK_EQUAL(nodeId1, g->getLinks()[0].from);
   BOOST_CHECK_EQUAL(nodeId2, g->getLinks()[0].to);
}

BOOST_FIXTURE_TEST_CASE( StrategyGraphTest_CreateCyclical_Triangle, StrategyGraphTest )
{
   auto graphId = mGoals[0].id;
   auto nodeId1 = mStrategy.createNode(graphId);
   auto nodeId2 = mStrategy.createNode(graphId);
   auto nodeId3 = mStrategy.createNode(graphId);

   mStrategy.createLink(graphId, nodeId1, nodeId2);
   mStrategy.createLink(graphId, nodeId2, nodeId3);

   auto g = mStrategy.getGraph(graphId);

   BOOST_CHECK_EQUAL(2, g->getLinks().size());

   mStrategy.createLink(graphId, nodeId3, nodeId1);

   g = mStrategy.getGraph(graphId);

   BOOST_CHECK_EQUAL(2, g->getLinks().size());
}

BOOST_FIXTURE_TEST_CASE( StrategyGraphTest_CreateCyclical_Complex, StrategyGraphTest )
{
   auto graphId = mGoals[0].id;
   auto nodeId1 = mStrategy.createNode(graphId);
   auto nodeId2 = mStrategy.createNode(graphId);
   auto nodeId3 = mStrategy.createNode(graphId);
   auto nodeId4 = mStrategy.createNode(graphId);
   auto nodeId5 = mStrategy.createNode(graphId);

   mStrategy.createLink(graphId, nodeId1, nodeId2);
   mStrategy.createLink(graphId, nodeId2, nodeId3);
   mStrategy.createLink(graphId, nodeId2, nodeId4);
   mStrategy.createLink(graphId, nodeId2, nodeId5);

   auto g = mStrategy.getGraph(graphId);

   BOOST_CHECK_EQUAL(4, g->getLinks().size());

   mStrategy.createLink(graphId, nodeId3, nodeId1);

   g = mStrategy.getGraph(graphId);

   BOOST_CHECK_EQUAL(4, g->getLinks().size());
}

BOOST_FIXTURE_TEST_CASE( StrategyGraphTest_DeleteLink, StrategyGraphTest )
{
   auto graphId = mGoals[0].id;
   auto nodeId1 = mStrategy.createNode(graphId);
   auto nodeId2 = mStrategy.createNode(graphId);

   mStrategy.createLink(graphId, nodeId1, nodeId2);

   auto g = mStrategy.getGraph(graphId);

   BOOST_CHECK_EQUAL(1, g->getLinks().size());
   
   mStrategy.breakLink(graphId, nodeId1, nodeId2);
   g = mStrategy.getGraph(graphId);

   BOOST_CHECK_EQUAL(0, g->getLinks().size());
}

BOOST_FIXTURE_TEST_CASE( StrategyGraphTest_DeleteNode, StrategyGraphTest )  
{
   auto graphId = mGoals[0].id;
   auto nodeId1 = mStrategy.createNode(graphId);
   auto nodeId2 = mStrategy.createNode(graphId);
   auto nodeId3 = mStrategy.createNode(graphId);
   
   auto nodes = mStrategy.getGraph(graphId)->getNodes();

   BOOST_CHECK(materia::contains_id(nodes, nodeId1));
   BOOST_CHECK(materia::contains_id(nodes, nodeId2));
   BOOST_CHECK(materia::contains_id(nodes, nodeId3));

   mStrategy.deleteNode(graphId, nodeId3);
   nodes = mStrategy.getGraph(graphId)->getNodes();

   BOOST_CHECK(materia::contains_id(nodes, nodeId1));
   BOOST_CHECK(materia::contains_id(nodes, nodeId2));
   BOOST_CHECK(!materia::contains_id(nodes, nodeId3));
}

BOOST_FIXTURE_TEST_CASE( StrategyGraphTest_DeleteGoalNode, StrategyGraphTest )  
{
   auto graphId = mGoals[0].id;
   auto goalNodeId = mStrategy.getGraph(graphId)->getNodes()[0].id;
   mStrategy.deleteNode(graphId, goalNodeId);

   auto nodes = mStrategy.getGraph(graphId)->getNodes();
   BOOST_CHECK(materia::contains_id(nodes, goalNodeId));
}

BOOST_FIXTURE_TEST_CASE( StrategyGraphTest_CannotLinkFromGoalNode, StrategyGraphTest )  
{
   auto graphId = mGoals[0].id;
   auto goalNodeId = mStrategy.getGraph(graphId)->getNodes()[0].id;
   auto nodeId1 = mStrategy.createNode(graphId);
   auto nodeId2 = mStrategy.createNode(graphId);

   mStrategy.createLink(graphId, goalNodeId, nodeId1);

   auto g = mStrategy.getGraph(graphId);
   BOOST_CHECK_EQUAL(0, g->getLinks().size());
}

BOOST_FIXTURE_TEST_CASE( StrategyGraphTest_SetAttrs_GoalNode, StrategyGraphTest )  
{
   auto graphId = mGoals[0].id;
   auto goalNodeId = mStrategy.getGraph(graphId)->getNodes()[0].id;
   mStrategy.setNodeAttributes(graphId, goalNodeId, materia::NodeType::Task, {});

   auto g = mStrategy.getGraph(graphId);
   auto nodes = g->getNodes();
   BOOST_CHECK_EQUAL(materia::NodeType::Goal, materia::find_by_id(nodes, goalNodeId)->type);
}

BOOST_FIXTURE_TEST_CASE( StrategyGraphTest_SetAttrs_Simple, StrategyGraphTest )
{
   auto graphId = mGoals[0].id;
   auto nodeId = mStrategy.createNode(graphId);

   materia::TNodeAttrs attrs;
   attrs[materia::NodeAttributeType::IS_DONE] = "1";
   attrs[materia::NodeAttributeType::BRIEF] = "test";
   mStrategy.setNodeAttributes(graphId, nodeId, materia::NodeType::Task, attrs);

   auto g = mStrategy.getGraph(graphId);
   auto nodes = g->getNodes();

   BOOST_CHECK_EQUAL(materia::NodeType::Task, materia::find_by_id(nodes, nodeId)->type);

   attrs = g->getNodeAttributes(nodeId);
   BOOST_CHECK_EQUAL("test", attrs[materia::NodeAttributeType::BRIEF]);
   BOOST_CHECK_EQUAL("1", attrs[materia::NodeAttributeType::IS_DONE]);
}

BOOST_FIXTURE_TEST_CASE( StrategyGraphTest_SetAttrs_Counters, StrategyGraphTest )
{
   auto graphId = mGoals[0].id;
   auto nodeId = mStrategy.createNode(graphId);
   materia::TNodeAttrs attrs;
   attrs[materia::NodeAttributeType::PROGRESS_CURRENT] = "3";
   attrs[materia::NodeAttributeType::BRIEF] = "test";
   attrs[materia::NodeAttributeType::PROGRESS_TOTAL] = "0";
   mStrategy.setNodeAttributes(graphId, nodeId, materia::NodeType::Counter, attrs);

   auto g = mStrategy.getGraph(graphId);
   auto nodes = g->getNodes();

   BOOST_CHECK_EQUAL(materia::NodeType::Counter, materia::find_by_id(nodes, nodeId)->type);

   attrs = g->getNodeAttributes(nodeId);
   BOOST_CHECK_EQUAL("test", attrs[materia::NodeAttributeType::BRIEF]);
   BOOST_CHECK_EQUAL("0", attrs[materia::NodeAttributeType::PROGRESS_TOTAL]);
   BOOST_CHECK_EQUAL("3", attrs[materia::NodeAttributeType::PROGRESS_CURRENT]);
}

BOOST_FIXTURE_TEST_CASE( StrategyGraphTest_SetAttrs_Reassign, StrategyGraphTest )
{
   auto graphId = mGoals[0].id;
   auto nodeId = mStrategy.createNode(graphId);

   materia::TNodeAttrs attrs;
   attrs[materia::NodeAttributeType::IS_DONE] = "1";
   attrs[materia::NodeAttributeType::BRIEF] = "test";
   mStrategy.setNodeAttributes(graphId, nodeId, materia::NodeType::Task, attrs);

   auto g = mStrategy.getGraph(graphId);
   auto nodes = g->getNodes();

   BOOST_CHECK_EQUAL(materia::NodeType::Task, materia::find_by_id(nodes, nodeId)->type);

   attrs = g->getNodeAttributes(nodeId);
   BOOST_CHECK_EQUAL("test", attrs[materia::NodeAttributeType::BRIEF]);
   BOOST_CHECK_EQUAL("1", attrs[materia::NodeAttributeType::IS_DONE]);

   attrs = materia::TNodeAttrs();

   attrs[materia::NodeAttributeType::PROGRESS_CURRENT] = "3";
   attrs[materia::NodeAttributeType::BRIEF] = "test";
   attrs[materia::NodeAttributeType::PROGRESS_TOTAL] = "0";
   mStrategy.setNodeAttributes(graphId, nodeId, materia::NodeType::Counter, attrs);

   g = mStrategy.getGraph(graphId);
   nodes = g->getNodes();

   BOOST_CHECK_EQUAL(materia::NodeType::Counter, materia::find_by_id(nodes, nodeId)->type);

   auto cattrs = g->getNodeAttributes(nodeId);
   BOOST_CHECK_EQUAL("test", cattrs[materia::NodeAttributeType::BRIEF]);
   BOOST_CHECK_EQUAL("0", cattrs[materia::NodeAttributeType::PROGRESS_TOTAL]);
   BOOST_CHECK_EQUAL("3", cattrs[materia::NodeAttributeType::PROGRESS_CURRENT]);
}

BOOST_FIXTURE_TEST_CASE( StrategyGraphTest_SetAttrs_WrongId, StrategyGraphTest )  
{
   auto graphId = mGoals[0].id;
   mStrategy.setNodeAttributes(graphId, materia::Id("invalid"), materia::NodeType::Task, {});
   mStrategy.setNodeAttributes(graphId, materia::Id("invalid"), materia::NodeType::Counter, {});
   mStrategy.setNodeAttributes(materia::Id("invalid"), materia::Id("invalid"), materia::NodeType::Counter, {});
}

BOOST_FIXTURE_TEST_CASE( StrategyGraphTest_DeleteNode_LinksDeleted, StrategyGraphTest )  
{
   auto graphId = mGoals[0].id;
   auto nodeId1 = mStrategy.createNode(graphId);
   auto nodeId2 = mStrategy.createNode(graphId);
   auto nodeId3 = mStrategy.createNode(graphId);

   mStrategy.createLink(graphId, nodeId1, nodeId2);
   mStrategy.createLink(graphId, nodeId2, nodeId3);

   mStrategy.deleteNode(graphId, nodeId2);

   auto g = mStrategy.getGraph(graphId);
   BOOST_CHECK_EQUAL(0, g->getLinks().size());
}