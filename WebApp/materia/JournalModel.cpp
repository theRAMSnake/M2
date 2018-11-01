#include "JournalModel.hpp"

JournalModel::JournalModel(ZmqPbChannel& channel)
: mService(channel)
{

}

std::vector<IndexItem> JournalModel::getChildren(const materia::Id& id)
{
   std::vector<IndexItem> result;

   auto loadedItems = loadIndex();
   std::copy_if(loadedItems.begin(), loadedItems.end(), std::inserter(result, result.begin()), [&](auto x)->bool {
      return x.parentFolderId == id;
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

   return *loadIndexItem(id);
}

std::optional<IndexItem> JournalModel::loadIndexItem(const materia::Id& id)
{
   auto loadedItems = loadIndex();
   auto iter = materia::find_by_id(loadedItems, id);

   if(iter != loadedItems.end())
   {
      return *iter;
   }

   return std::optional<IndexItem>();
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
      newItem.parentFolderId = x.journalitem().folderid().guid();
      newItem.title = x.journalitem().title();

      result.push_back(newItem);
   }

   return result;
}

void JournalModel::renameIndexItem(const materia::Id& id, const std::string& name)
{
   auto newItem = *loadIndexItem(id);
   newItem.title = name;

   journal::JournalItem params;
   params.mutable_id()->set_guid(newItem.id.getGuid());
   params.mutable_folderid()->set_guid(newItem.parentFolderId.getGuid());
   params.set_title(name);

   common::OperationResultMessage dummy;
   mService.getService().UpdateIndexItem(nullptr, &params, &dummy, nullptr);
}

void JournalModel::deleteItem(const materia::Id& id)
{
   common::UniqueId request;
   request.set_guid(id.getGuid());
   common::OperationResultMessage dummy;
   mService.getService().DeleteItem(nullptr, &request, &dummy, nullptr);
}

void JournalModel::saveContent(const materia::Id& id, const std::string& content)
{
   common::UniqueId request;
   request.set_guid(id.getGuid());
   journal::Page page;
   mService.getService().GetPage(nullptr, &request, &page, nullptr);

   page.set_content(content);

   common::OperationResultMessage dummy;
   mService.getService().UpdatePage(nullptr, &page, &dummy, nullptr);
}

std::string JournalModel::loadContent(const materia::Id& id)
{
   common::UniqueId request;
   request.set_guid(id.getGuid());
   journal::Page page;
   mService.getService().GetPage(nullptr, &request, &page, nullptr);

   return page.content();
}