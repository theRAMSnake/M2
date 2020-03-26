#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <Core/ICore.hpp>
#include <Core/IChallenge.hpp>

class ChallengeTest
{
public:
   ChallengeTest()
   {
      system("rm Test.db");
      mCore = materia::createCore({"Test.db"});
      mCh = &mCore->getChallenge();
   }

protected:

   std::shared_ptr<materia::ICore> mCore;
   materia::IChallenge* mCh;
};

BOOST_FIXTURE_TEST_CASE( AddDeleteCh, ChallengeTest ) 
{
   BOOST_CHECK(mCh->addChallenge("ch", 5) != materia::Id::Invalid);

   auto items = mCh->get();
   BOOST_CHECK_EQUAL(1, items.size());
   BOOST_CHECK_EQUAL("ch", items[0].name);
   BOOST_CHECK_EQUAL(1, items[0].level);
   BOOST_CHECK_EQUAL(5, items[0].maxLevels);

   mCh->removeChallenge(items[0].id);

   items = mCh->get();
   BOOST_CHECK_EQUAL(0, items.size());
}

BOOST_FIXTURE_TEST_CASE( AddRemoveLayer, ChallengeTest ) 
{
    auto id = mCh->addChallenge("ch", 5);

    auto idStages = mCh->addLayer(id, {materia::Id::Invalid, materia::StagesLayer{std::vector<bool>(5)}});
    auto idPoints = mCh->addLayer(id, {materia::Id::Invalid, materia::PointsLayer{0, 10, 5, materia::PointsLayerType::Total}});

    auto items = mCh->get();
    BOOST_CHECK_EQUAL(1, items.size());

    auto layers = items[0].layers;
    BOOST_CHECK_EQUAL(2, layers.size());

    BOOST_CHECK_EQUAL(idStages, layers[0].id);
    BOOST_CHECK(std::holds_alternative<materia::StagesLayer>(layers[0].parameters));

    BOOST_CHECK_EQUAL(5, std::get<materia::StagesLayer>(layers[0].parameters).stages.size());

    BOOST_CHECK_EQUAL(idPoints, layers[1].id);
    BOOST_CHECK(std::holds_alternative<materia::PointsLayer>(layers[1].parameters));

    auto& pointsLayer = std::get<materia::PointsLayer>(layers[1].parameters);
    BOOST_CHECK_EQUAL(0, pointsLayer.numPoints);
    BOOST_CHECK_EQUAL(10, pointsLayer.pointsToNextLevel);
    BOOST_CHECK_EQUAL(5, pointsLayer.advancementValue);
    BOOST_CHECK_EQUAL(static_cast<int>(materia::PointsLayerType::Total), static_cast<int>(pointsLayer.type));

    mCh->removeLayer(id, idStages);
    mCh->removeLayer(id, idPoints);

    items = mCh->get();
    BOOST_CHECK_EQUAL(1, items.size());

    layers = items[0].layers;
    BOOST_CHECK_EQUAL(0, layers.size());
}

BOOST_FIXTURE_TEST_CASE( ToggleStage, ChallengeTest ) 
{
    auto id = mCh->addChallenge("ch", 5);
    auto idStages = mCh->addLayer(id, {materia::Id::Invalid, materia::StagesLayer{std::vector<bool>(5)}});

    mCh->toggleStage(id, idStages, 6); //Check no crash
    mCh->toggleStage(id, idStages, 1);
    mCh->toggleStage(id, idStages, 2);
    mCh->toggleStage(id, idStages, 3);

    auto items = mCh->get();
    BOOST_CHECK_EQUAL(1, items.size());

    auto layers = items[0].layers;
    BOOST_CHECK_EQUAL(1, layers.size());

    auto stages = std::get<materia::StagesLayer>(layers[0].parameters).stages;
    BOOST_CHECK(!stages[0]);
    BOOST_CHECK(stages[1]);
    BOOST_CHECK(stages[2]);
    BOOST_CHECK(stages[3]);
    BOOST_CHECK(!stages[4]);

    BOOST_CHECK_EQUAL(1, items[0].level);

    mCh->toggleStage(id, idStages, 0);
    mCh->toggleStage(id, idStages, 3);
    mCh->toggleStage(id, idStages, 4);

    items = mCh->get();
    BOOST_CHECK_EQUAL(1, items.size());

    layers = items[0].layers;
    BOOST_CHECK_EQUAL(1, layers.size());

    stages = std::get<materia::StagesLayer>(layers[0].parameters).stages;
    BOOST_CHECK(stages[0]);
    BOOST_CHECK(stages[1]);
    BOOST_CHECK(stages[2]);
    BOOST_CHECK(!stages[3]);
    BOOST_CHECK(stages[4]);

    BOOST_CHECK_EQUAL(1, items[0].level);

    mCh->toggleStage(id, idStages, 3);

    items = mCh->get();
    BOOST_CHECK_EQUAL(1, items.size());

    layers = items[0].layers;
    BOOST_CHECK_EQUAL(1, layers.size());

    stages = std::get<materia::StagesLayer>(layers[0].parameters).stages;
    BOOST_CHECK(!stages[0]);
    BOOST_CHECK(!stages[1]);
    BOOST_CHECK(!stages[2]);
    BOOST_CHECK(!stages[3]);
    BOOST_CHECK(!stages[4]);

    BOOST_CHECK_EQUAL(2, items[0].level);
}

BOOST_FIXTURE_TEST_CASE( AddPoints_ch, ChallengeTest ) 
{
    auto id = mCh->addChallenge("ch", 5);
    auto idPoints = mCh->addLayer(id, {materia::Id::Invalid, materia::PointsLayer{0, 10, 5, materia::PointsLayerType::Total}});

    mCh->addPoints(id, idPoints, 7);

    auto items = mCh->get();
    BOOST_CHECK_EQUAL(1, items.size());

    auto layers = items[0].layers;
    BOOST_CHECK_EQUAL(1, layers.size());

    auto pts = std::get<materia::PointsLayer>(layers[0].parameters);
    BOOST_CHECK_EQUAL(7, pts.numPoints);
    BOOST_CHECK_EQUAL(1, items[0].level);

    mCh->addPoints(id, idPoints, 4);

    items = mCh->get();
    BOOST_CHECK_EQUAL(1, items.size());

    layers = items[0].layers;
    BOOST_CHECK_EQUAL(1, layers.size());

    pts = std::get<materia::PointsLayer>(layers[0].parameters);
    BOOST_CHECK_EQUAL(1, pts.numPoints);
    BOOST_CHECK_EQUAL(15, pts.pointsToNextLevel);
    BOOST_CHECK_EQUAL(2, items[0].level);
}

BOOST_FIXTURE_TEST_CASE( WeeklyTest, ChallengeTest ) 
{
    auto id = mCh->addChallenge("ch", 5);
    auto idPoints = mCh->addLayer(id, {materia::Id::Invalid, materia::PointsLayer{0, 10, 5, materia::PointsLayerType::Weekly}});
    mCh->addPoints(id, idPoints, 7);

    mCore->onNewWeek();

    auto items = mCh->get();
    BOOST_CHECK_EQUAL(1, items.size());

    auto layers = items[0].layers;
    BOOST_CHECK_EQUAL(1, layers.size());

    auto pts = std::get<materia::PointsLayer>(layers[0].parameters);
    BOOST_CHECK_EQUAL(0, pts.numPoints);
    BOOST_CHECK_EQUAL(1, items[0].level);
}