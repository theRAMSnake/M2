#pragma once

#include <Common/Id.hpp>
#include <messages/challenge.pb.h>
#include "ZmqPbChannel.hpp"
#include "MateriaServiceProxy.hpp"

struct ChallengeLayer
{
    /*SNAKE*/
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