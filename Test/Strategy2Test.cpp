#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <Core/ICore.hpp>
#include <Core/IStrategy_v2.hpp>

extern std::shared_ptr<materia::ICore> createTestCore();

class Strategy2Test
{
public:
   Strategy2Test()
   : mCore(createTestCore())
   , mStrategy(mCore->getStrategy_v2())
   {
      
   }

protected:

   std::shared_ptr<materia::ICore> mCore;
   materia::IStrategy_v2& mStrategy;
};

BOOST_FIXTURE_TEST_CASE( AddDeleteWatchItem, Strategy2Test ) 
{  
   BOOST_CHECK(mStrategy.addWatchItem({materia::Id::Invalid, "text"}) != materia::Id::Invalid);

   auto items = mStrategy.getWatchItems();
   BOOST_CHECK_EQUAL(1, items.size());
   BOOST_CHECK_EQUAL("text", items[0].text);

   mStrategy.removeWatchItem(items[0].id);
   
   items = mStrategy.getWatchItems();
   BOOST_CHECK_EQUAL(0, items.size());
}

BOOST_FIXTURE_TEST_CASE( EditWatchItem, Strategy2Test ) 
{
   materia::Id newId = mStrategy.addWatchItem({materia::Id::Invalid, "text"});
   BOOST_CHECK(newId != materia::Id::Invalid);

   mStrategy.replaceWatchItem({newId, "other_text"});

   auto items = mStrategy.getWatchItems();
   BOOST_CHECK_EQUAL(1, items.size());
   BOOST_CHECK_EQUAL("other_text", items[0].text);
   BOOST_CHECK_EQUAL(newId, items[0].id);
}

BOOST_FIXTURE_TEST_CASE( AddDeleteFocusItem, Strategy2Test ) 
{
   auto id1 = mStrategy.addFocusItem({materia::Id("1"), materia::Id("1")});
   auto id2 = mStrategy.addFocusItem({materia::Id("1"), materia::Id("2")});
   auto id3 = mStrategy.addFocusItem({materia::Id("1"), materia::Id("3")});

   auto items = mStrategy.getFocusItems();
   BOOST_CHECK_EQUAL(3, items.size());
   BOOST_CHECK_EQUAL(materia::Id("1"), materia::find_by_id(items, id1)->graphId);
   BOOST_CHECK_EQUAL(materia::Id("1"), materia::find_by_id(items, id2)->graphId);
   BOOST_CHECK_EQUAL(materia::Id("1"), materia::find_by_id(items, id3)->graphId);
   BOOST_CHECK_EQUAL(materia::Id("1"), materia::find_by_id(items, id1)->nodeId);
   BOOST_CHECK_EQUAL(materia::Id("2"), materia::find_by_id(items, id2)->nodeId);
   BOOST_CHECK_EQUAL(materia::Id("3"), materia::find_by_id(items, id3)->nodeId);

   mStrategy.removeFocusItem(items[0].id);
   
   items = mStrategy.getFocusItems();
   BOOST_CHECK_EQUAL(2, items.size());
   BOOST_CHECK_EQUAL(materia::Id("1"), materia::find_by_id(items, id2)->graphId);
   BOOST_CHECK_EQUAL(materia::Id("1"), materia::find_by_id(items, id3)->graphId);
   BOOST_CHECK_EQUAL(materia::Id("2"), materia::find_by_id(items, id2)->nodeId);
   BOOST_CHECK_EQUAL(materia::Id("3"), materia::find_by_id(items, id3)->nodeId);
}

BOOST_FIXTURE_TEST_CASE( CompleteFocusItemTask, Strategy2Test ) 
{
   auto gId = mStrategy.addGoal({});

   auto nodeId = mStrategy.createNode(gId);

   materia::NodeAttributes attrs;
   attrs.set<materia::NodeAttributeType::IS_DONE>(false);
   attrs.set<materia::NodeAttributeType::BRIEF>("test");
   mStrategy.setNodeAttributes(gId, nodeId, materia::NodeType::Task, attrs);

   auto id = mStrategy.addFocusItem({gId, nodeId});
   mStrategy.completeFocusItem({gId, nodeId, id});

   auto g = mStrategy.getGraph(gId);
   attrs = g->getNodeAttributes(nodeId);
   BOOST_CHECK_EQUAL(true, attrs.get<materia::NodeAttributeType::IS_DONE>());
}

BOOST_FIXTURE_TEST_CASE( CompleteFocusItemCounter, Strategy2Test ) 
{
   auto gId = mStrategy.addGoal({});

   auto nodeId = mStrategy.createNode(gId);

   materia::NodeAttributes attrs;
   attrs.set<materia::NodeAttributeType::PROGRESS_CURRENT>(0);
   attrs.set<materia::NodeAttributeType::PROGRESS_TOTAL>(3);
   attrs.set<materia::NodeAttributeType::BRIEF>("test");
   mStrategy.setNodeAttributes(gId, nodeId, materia::NodeType::Counter, attrs);

   {
      auto id = mStrategy.addFocusItem({gId, nodeId});
      mStrategy.completeFocusItem({gId, nodeId, id});

      auto g = mStrategy.getGraph(gId);
      attrs = g->getNodeAttributes(nodeId);
      BOOST_CHECK_EQUAL(1, attrs.get<materia::NodeAttributeType::PROGRESS_CURRENT>());
   }
   {
      auto id = mStrategy.addFocusItem({gId, nodeId});
      mStrategy.completeFocusItem({gId, nodeId, id});

      auto g = mStrategy.getGraph(gId);
      attrs = g->getNodeAttributes(nodeId);
      BOOST_CHECK_EQUAL(2, attrs.get<materia::NodeAttributeType::PROGRESS_CURRENT>());
   }
}

BOOST_FIXTURE_TEST_CASE( CompleteFocusItemOther, Strategy2Test ) 
{
   auto gId = mStrategy.addGoal({});

   auto nodeId = mStrategy.createNode(gId);

   materia::NodeAttributes attrs;
   attrs.set<materia::NodeAttributeType::REQUIRED_TIMESTAMP>(std::time_t(std::numeric_limits<std::time_t>::max));
   attrs.set<materia::NodeAttributeType::BRIEF>("test");
   mStrategy.setNodeAttributes(gId, nodeId, materia::NodeType::Wait, attrs);

   auto id = mStrategy.addFocusItem({gId, nodeId});
   mStrategy.completeFocusItem({gId, nodeId, id});

   auto g = mStrategy.getGraph(gId);
   attrs = g->getNodeAttributes(nodeId);
   BOOST_CHECK_EQUAL(false, attrs.get<materia::NodeAttributeType::IS_DONE>());
}