#pragma once

#include <Common/Id.hpp>

namespace materia
{

struct RewardPoolItem
{
   Id id;
   std::string name;
   unsigned int amount = 0;
   unsigned int amountMax = 0;
}

class IReward
{
public:
   virtual void addPoints(const unsigned int number) = 0;
   virtual void spendFromPool(const Id poolId) = 0;

   virtual std::vector<RewardPoolItem> getPools() const = 0;
   virtual void removePool(const Id& id) = 0;
   virtual Id addPool(const RewardPoolItem& item) = 0;
   virtual void modifyPool(const RewardPoolItem& item) = 0;

   virtual ~IReward(){}
};

}