#pragma once

#include "../IInbox.hpp"

namespace materia
{

class Inbox : public IInbox
{
public:
   Inbox(Database& db);

   std::vector<InboxItem> getItems() override;
   void remove(const Id& id) override;
   void replace(const InboxItem& item) override;
   Id add(const InboxItem& item) override;

private:
   std::vector<InboxItem> mItems;
   Database mDb;
};

}