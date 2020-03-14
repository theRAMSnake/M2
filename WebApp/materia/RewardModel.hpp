#pragma once

#include <Common/Id.hpp>
#include <messages/reward.pb.h>
#include "ZmqPbChannel.hpp"
#include "MateriaServiceProxy.hpp"

class RewardModel
{
public:
   RewardModel(ZmqPbChannel& channel);

   struct Item
   {
      materia::Id id;
      std::string name;
      unsigned int amount;
      unsigned int amountMax;
   };

   std::vector<Item> get();
   materia::Id add(const Item& item);
   void replace(const Item& item);
   void erase(const materia::Id& id);

   void addPoints(const unsigned int amount);

private:
   MateriaServiceProxy<reward::RewardService> mService;
};