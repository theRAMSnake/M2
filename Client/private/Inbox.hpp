#pragma once

#include "MateriaServiceProxy.hpp"
#include "../IInbox.hpp"

#include "messages/inbox.pb.h"

namespace materia
{

class Inbox : public IInbox
{
public:
   Inbox(materia::ZmqPbChannel& channel);

   std::vector<InboxItem> getItems() override;
   bool deleteItem(const Id& id) override;
   bool replaceItem(const InboxItem& item) override;
   Id insertItem(const InboxItem& item) override;

private:
   MateriaServiceProxy<inbox::InboxService> mProxy;
};

}