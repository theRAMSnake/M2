#pragma once

#include "../IInbox.hpp"
#include "Database.hpp"

namespace materia
{

class Database;
class Inbox : public IInbox
{
public:
   Inbox(Database& db);

   std::vector<InboxItem> get() const override;
   void remove(const Id& id) override;
   void replace(const InboxItem& item) override;
   Id add(const InboxItem& item) override;

private:
   std::vector<InboxItem> mItems;
   std::unique_ptr<DatabaseTable> mStorage;
};

}