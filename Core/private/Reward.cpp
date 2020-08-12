#include "Reward.hpp"
#include "JsonSerializer.hpp"

BIND_JSON4(materia::RewardPoolItem, id, name, amount, amountMax)

namespace materia
{

Reward::Reward(Database& db)
: mStorage(db.getTable("reward"))
{

}

void Reward::addPoints(const unsigned int number)
{
   srand(time(0));

   auto pools = getPools();

   unsigned int attemptCounter = 0;
   unsigned int pointsLeft = number;
   while(!pools.empty() && pointsLeft > 0)
   {
      auto& randomItem = pools[rand() % pools.size()];

      if(randomItem.amount < randomItem.amountMax)
      {
         randomItem.amount++;
         pointsLeft--;
      }
      else
      {
         attemptCounter++;
         if(attemptCounter == 100)
         {
            break;
         }

         continue;
      }
   }

   for(auto& p : pools)
   {
      modifyPool(p);
   }
}

void Reward::removePoints(const unsigned int number)
{
   srand(time(0));

   auto pools = getPools();

   unsigned int attemptCounter = 0;
   unsigned int pointsLeft = number;
   while(!pools.empty() && pointsLeft > 0)
   {
      auto& randomItem = pools[rand() % pools.size()];

      if(randomItem.amount > 0)
      {
         randomItem.amount--;
         pointsLeft--;
      }
      else
      {
         attemptCounter++;
         if(attemptCounter == 100)
         {
            break;
         }

         continue;
      }
   }

   for(auto& p : pools)
   {
      modifyPool(p);
   }
}

std::vector<RewardPoolItem> Reward::getPools() const
{
   std::vector<RewardPoolItem> result;

   mStorage->foreach([&](std::string id, std::string json) 
   {
      result.push_back(readJson<RewardPoolItem>(json));
   });

   return result;
}

void Reward::removePool(const Id& id)
{
   mStorage->erase(id);
}

Id Reward::addPool(const RewardPoolItem& item)
{
   auto newItem = item;
   newItem.id = Id::generate();

   mStorage->store(newItem.id, writeJson(newItem));

   return newItem.id;
}

void Reward::modifyPool(const RewardPoolItem& item)
{
   mStorage->store(item.id, writeJson(item));
}

}
