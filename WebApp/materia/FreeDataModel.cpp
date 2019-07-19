#include "FreeDataModel.hpp"

FreeDataModel::FreeDataModel(ZmqPbChannel& channel)
: mService(channel)
{

}

std::optional<FreeDataModel::Block> FreeDataModel::get(const std::string& name)
{
   std::optional<FreeDataModel::Block> result;

   auto items = get();
   auto pos = std::find_if(items.begin(), items.end(), [&](auto x)->bool {return x.name == name;});

   if(pos != items.end())
   {
      result = *pos;
   }

   return result;
}

std::vector<FreeDataModel::Block> FreeDataModel::get()
{
   std::vector<FreeDataModel::Block> result;

   common::EmptyMessage e;
   freedata::FreeDataBlocks items;

   mService.getService().Get(nullptr, &e, &items, nullptr);

   for(auto x : items.items())
   {
      result.push_back({x.name(), x.value()});
   }

   return result;
}

void FreeDataModel::set(const Block& b)
{
   freedata::FreeDataBlock block;
   block.set_name(b.name);
   block.set_value(b.value);

   common::OperationResultMessage dummy;
   mService.getService().Set(nullptr, &block, &dummy, nullptr);
}

void FreeDataModel::remove(const std::string& blockname)
{
   common::StringMessage str;
   str.set_content(blockname);

   common::OperationResultMessage dummy;
   mService.getService().Remove(nullptr, &str, &dummy, nullptr);
}