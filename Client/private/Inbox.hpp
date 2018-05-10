#pragma once

#include "MateriaServiceProxy.hpp"
#include "Id.hpp"

#include "messages/inbox.pb.h"

#include <boost/optional.hpp>

namespace materia
{

struct InboxItem
{
   Id id;
   std::string text;
};

class Inbox
{
public:
   Inbox(materia::ZmqPbChannel& channel);

   std::vector<InboxItem> getItems();
   bool deleteItem(const Id& id);
   bool replaceItem(const InboxItem& item);
   Id insertItem(const InboxItem& item);

private:
   MateriaServiceProxy<inbox::InboxService> mProxy;
};

}