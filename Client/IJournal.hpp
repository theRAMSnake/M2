/*
   rpc InsertFolder (InsertFolderParams) returns (common.UniqueId);
   rpc InsertPage (InsertPageParams) returns (common.UniqueId);

   rpc DeleteItem (common.UniqueId) returns (common.OperationResultMessage);

   rpc UpdateFolder (JournalItem) returns (common.OperationResultMessage);
   rpc UpdatePage (Page) returns (common.OperationResultMessage); //Don't forget to test timestamp

   rpc GetIndex (common.EmptyMessage) returns (Index);
   rpc Search (common.StringMessage) returns (SearchResult);
   rpc GetPage (common.UniqueId) returns (Page);
*/

#pragma once

#include "Id.hpp"
#include <ctime>
#include <vector>

namespace materia
{

struct JournalItem
{
   Id id;
   Id parentFolderId;
   std::string title;
};

struct JournalPage : public JournalItem
{
   std::string content;
};

struct IndexItem : public JournalItem
{
   std::time_t modified;
   bool isPage;

   bool operator == (const IndexItem& other) const;
};

struct SearchResult
{
   Id pageId;
   std::size_t position;
};

class IJournal
{
public:
   virtual Id insertFolder(const Id& parentFolderId, const std::string& name) = 0;
   virtual Id insertPage(const Id& parentFolderId, const std::string& name, const std::string& content) = 0;

   virtual bool deleteItem(const Id& id) = 0;

   virtual bool updateFolder(const JournalItem& item) = 0;
   virtual bool updatePage(const JournalPage& item) = 0;

   virtual std::vector<IndexItem> getIndex() = 0;
   virtual std::vector<SearchResult> search(const std::string& keyword) = 0;
   virtual std::optional<JournalPage> getPage(const Id& id) = 0;

   virtual void clear() = 0;

   virtual ~IJournal(){}
};

}