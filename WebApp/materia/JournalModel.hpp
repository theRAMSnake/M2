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

class JournalModel
{
public:
   JournalModel(ZmqPbChannel& channel);

   std::vector<IndexItem> getChildren(const materia::Id& id);
   IndexItem addIndexItem(const bool isPage, const std::string& name, const materia::Id& parentId);

private:
   std::vector<IndexItem> loadIndex();
   std::optional<IndexItem> loadIndexItem(const materia::Id& id);

   MateriaServiceProxy<journal::JournalService> mService;
};