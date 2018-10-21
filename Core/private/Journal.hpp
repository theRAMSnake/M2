#pragma once

#include "../Id.hpp"
#include "../IJournal.hpp"
#include "Database.hpp"
#include <map>

namespace materia
{

class Database;
class Journal : public IJournal
{
public:
   Journal(Database& db);

   Id insertFolder(const Id& parentFolderId, const std::string& name) override;
   Id insertPage(const Id& parentFolderId, const std::string& name, const std::string& content) override;

   void deleteItem(const Id& id) override;

   void updateFolder(const JournalItem& item) override;
   void updatePage(const JournalPage& item) override;

   std::vector<IndexItem> getIndex() override;
   std::vector<SearchResult> search(const std::string& keyword) override;
   std::optional<JournalPage> getPage(const Id& id) override;

private:
   const Id insertIndexItem(const Id& parentId, const std::string& title, const bool isPage);
   bool isFolderExist(const Id& id);
   void updateIndexItem(const JournalItem& item);

   std::map<Id, IndexItem> mIndex;
   std::map<Id, std::string> mPageContents;

   std::unique_ptr<DatabaseTable> mIndexStorage;
   std::unique_ptr<DatabaseTable> mContentStorage;
};

}