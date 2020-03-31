#pragma once

#include <Common/Id.hpp>
#include <messages/challenge.pb.h>
#include "ZmqPbChannel.hpp"
#include "MateriaServiceProxy.hpp"

#include <boost/signals2.hpp>

#include <variant>

struct PointsLayer
{
   unsigned int points;
   unsigned int pointsNeeded;
   unsigned int pointsAdvance;
   challenge::PointsLayerType type;
   unsigned int newPoints = 0;
};

struct StagesLayer
{
   std::vector<bool> stages;
};

struct ChallengeLayer
{
   materia::Id id;
   std::variant<StagesLayer, PointsLayer> params;
};

class ChallengeModel
{
public:
   boost::signals2::signal<void()> changed;

   ChallengeModel(ZmqPbChannel& channel);

   struct Item
   {
      materia::Id id;
      std::string name;
      unsigned int level;
      unsigned int levelMax;
      std::vector<ChallengeLayer> layers;
   };

   std::vector<Item> get();

   void createChallenge(const std::string& name, const unsigned int maxLevel);

   void eraseLayer(materia::Id chId, materia::Id lId);
   void eraseChallenge(materia::Id chId);
   void apply(materia::Id chId, materia::Id lId, const PointsLayer& p);
   void apply(materia::Id chId, materia::Id lId, const StagesLayer& p);

   void createPointsLayer(materia::Id chId, const PointsLayer& p);
   void createStagesLayer(materia::Id chId, const unsigned int& numStages);

private:
   std::vector<ChallengeModel::Item> mItems;
   MateriaServiceProxy<challenge::ChallengeService> mService;
};