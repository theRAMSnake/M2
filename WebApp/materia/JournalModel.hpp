#pragma once

#include <Common/Id.hpp>
#include <messages/journal.pb.h>
#include "ZmqPbChannel.hpp"
#include "MateriaServiceProxy.hpp"

struct JournalItem
{
   materia::Id id;
   materia::Id parentFolderId;
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
};

struct SearchResult
{
   materia::Id pageId;
   std::size_t position;
};

std::vector<std::string> parsePagetoList(const std::string& content);

class JournalModel
{
public:
   JournalModel(ZmqPbChannel& channel);

   std::vector<IndexItem> getChildren(const materia::Id& id);
   IndexItem addIndexItem(const bool isPage, const std::string& name, const materia::Id& parentId);
   void renameIndexItem(const materia::Id& id, const std::string& name);
   void deleteItem(const materia::Id& id);
   materia::Id searchIndex(const std::string& name);

   void saveContent(const materia::Id& id, const std::string& content);
   std::string loadContent(const materia::Id& id);

   std::string getTipOfTheDay();

private:
   std::vector<IndexItem> loadIndex();
   std::optional<IndexItem> loadIndexItem(const materia::Id& id);

   MateriaServiceProxy<journal::JournalService> mService;
};