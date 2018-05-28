#pragma once

#include "MateriaServiceProxy.hpp"
#include <messages/journal.pb.h>
#include "../Id.hpp"
#include "../IJournal.hpp"

namespace materia
{

class Journal : public IJournal
{
public:
   Journal(materia::ZmqPbChannel& channel);

   Id insertFolder(const Id& parentFolderId, const std::string& name) override;
   Id insertPage(const Id& parentFolderId, const std::string& name, const std::string& content) override;

   bool deleteItem(const Id& id) override;

   bool updateFolder(const JournalItem& item) override;
   bool updatePage(const JournalPage& item) override;

   std::vector<IndexItem> getIndex() override;
   std::vector<SearchResult> search(const std::string& keyword) override;
   std::optional<JournalPage> getPage(const Id& id) override;

   void clear() override;

private:
   MateriaServiceProxy<journal::JournalService> mProxy;
};

}