#pragma once

#include "../IReward.hpp"
#include "Database.hpp"
#include <memory>

namespace materia
{

class Database;
class Reward : public IReward
{
public:
   Reward(Database& db);

   void addPoints(const unsigned int number) override;

   std::vector<RewardPoolItem> getPools() const override;
   void removePool(const Id& id) override;
   Id addPool(const RewardPoolItem& item) override;
   void modifyPool(const RewardPoolItem& item) override;

private:
   std::unique_ptr<DatabaseTable> mStorage;
};

}