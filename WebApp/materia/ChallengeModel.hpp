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
   challenge::PointsLayerType type;
};

struct StagesLayer
{
   /*SNAKE*/
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

private:
   MateriaServiceProxy<challenge::ChallengeService> mService;
};