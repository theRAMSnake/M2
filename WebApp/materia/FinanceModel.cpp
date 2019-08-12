#include "FinanceModel.hpp"

FinanceModel::FinanceModel(ZmqPbChannel& channel)
: mService(channel)
{

}

std::vector<FinanceModel::Category> FinanceModel::getCategories()
{
   common::EmptyMessage e;

   finance::CategoryItems items;

   mService.getService().GetCategories(nullptr, &e, &items, nullptr);

   std::vector<Category> result;

   for(auto x : items.items())
   {
      result.push_back({x.id().guid(), x.name()});
   }

   return result;
}

void FinanceModel::renameCategory(const materia::Id& id, const std::string& newName)
{
   finance::CategoryInfo itemToEdit;
   itemToEdit.mutable_id()->set_guid(id);
   itemToEdit.set_name(newName);

   common::OperationResultMessage dummy;
   mService.getService().ReplaceCategory(nullptr, &itemToEdit, &dummy, nullptr);
}

void FinanceModel::eraseCategory(const materia::Id& id)
{
   common::UniqueId idMsg;
   idMsg.set_guid(id.getGuid());

   common::OperationResultMessage dummy;
   mService.getService().DeleteCategory(nullptr, &idMsg, &dummy, nullptr);
}

materia::Id FinanceModel::addCategory(const std::string& name)
{
   finance::CategoryInfo itemToAdd;
   itemToAdd.set_name(name);

   common::UniqueId id;
   mService.getService().AddCategory(nullptr, &itemToAdd, &id, nullptr);

   return id.guid();
}