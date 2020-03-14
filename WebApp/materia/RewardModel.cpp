#include "RewardModel.hpp"

RewardModel::RewardModel(ZmqPbChannel& channel)
: mService(channel)
{

}

std::vector<RewardModel::Item> RewardModel::get()
{
   common::EmptyMessage e;

   reward::RewardPoolItems items;

   mService.getService().GetPools(nullptr, &e, &items, nullptr);

   std::vector<Item> result;

   for(auto x : items.items())
   {
      result.push_back({x.id().guid(), x.name(), x.amount(), x.amountmax()});
   }

   return result;
}

materia::Id RewardModel::add(const Item& item)
{
   reward::RewardPoolItem itemToAdd;
   itemToAdd.set_name(item.name);
   itemToAdd.set_amount(item.amount);
   itemToAdd.set_amountmax(item.amountMax);

   common::UniqueId id;
   mService.getService().AddPool(nullptr, &itemToAdd, &id, nullptr);

   return id.guid();
}

void RewardModel::replace(const Item& item)
{
   reward::RewardPoolItem itemToEdit;
   itemToEdit.mutable_id()->set_guid(item.id.getGuid());
   itemToEdit.set_name(item.name);
   itemToEdit.set_amount(item.amount);
   itemToEdit.set_amountmax(item.amountMax);

   common::OperationResultMessage dummy;
   mService.getService().EditPool(nullptr, &itemToEdit, &dummy, nullptr);
}

void RewardModel::erase(const materia::Id& id)
{
   common::UniqueId idMsg;
   idMsg.set_guid(id.getGuid());

   common::OperationResultMessage dummy;
   mService.getService().DeletePool(nullptr, &idMsg, &dummy, nullptr);
}

void RewardModel::addPoints(const unsigned int amount)
{
   common::IntMessage intMsg;
   intMsg.set_content(amount);

   common::OperationResultMessage dummy;
   mService.getService().AddPoints(nullptr, &intMsg, &dummy, nullptr);
}