#pragma once

#include "Id.hpp"

namespace materia
{

struct InboxItem
{
   Id id;
   std::string text;
};

class IInbox
{
public:
   virtual std::vector<InboxItem> getItems() = 0;
   virtual bool deleteItem(const Id& id) = 0;
   virtual bool replaceItem(const InboxItem& item) = 0;
   virtual Id insertItem(const InboxItem& item) = 0;

   virtual ~IInbox(){}
};

}