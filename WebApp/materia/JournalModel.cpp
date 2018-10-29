#include "JournalModel.hpp"

JournalModel::JournalModel(ZmqPbChannel& channel)
: mService(channel)
{

}

std::vector<IndexItem> JournalModel::getChildren(const materia::Id& id)
{
   std::vector<IndexItem> result;

   auto loadedItems = loadIndex();
   std::copy_if(loadedItems.begin(), loadedItems.end(), std::inserter(result, result.begin()), [](auto x)->bool {
      return x.parentFolderI == id;
   });

   return result;
}

IndexItem JournalModel::addIndexItem(const bool isPage, const std::string& name, const materia::Id& parentId)
{
   materia::Id id;

   if(isPage)
   {
      journal::InsertPageParams params;
      params.mutable_parent_folder_id()->set_guid(parentId.getGuid());
      params.set_title(name);

      common::UniqueId newId;

      mService.getService().InsertPage(nullptr, &params, &newId, nullptr);

      id = newId.guid();
   }
   else
   {
      journal::InsertFolderParams params;
      params.mutable_parent_folder_id()->set_guid(parentId.getGuid());
      params.set_title(name);

      common::UniqueId newId;

      mService.getService().InsertFolder(nullptr, &params, &newId, nullptr);

      id = newId.guid();
   }

   return loadIndexItem(id);
}

IndexItem JournalModel::loadIndexItem(const materia::Id& id)
{
   auto loadedItems = loadIndex();
   materia::find_by_id();
}

std::vector<IndexItem> JournalModel::loadIndex()
{
   std::vector<IndexItem> result;

   common::EmptyMessage empty;
   journal::Index index;
   mService.getService().GetIndex(nullptr, &empty, &index, nullptr);

   for(auto x : index.items())
   {
      IndexItem newItem;
      newItem.id = x.journalitem().id().guid();
      newItem.isPage = x.ispage();
      newItem.modified = x.modifiedtimestamp();
      newItem.parentFolderId = x.journalitem().folderid();
      newItem.title = x.journalitem().title();

      result.push_back(newItem);
   }

   return result;
}