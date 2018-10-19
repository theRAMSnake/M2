#include "Inbox.hpp"

namespace materia
{

Inbox::Inbox(Database& db)
: mDb(db)
{
    loadItems();
}

std::vector<InboxItem> Inbox::get()
{
   return mItems;
}

void Inbox::remove(const Id& id)
{
   auto pos = find_by_id(mItems, id);
   if(pos != mItems.end())
   {
       mItems.erase(pos);
       mDb.erase(id);
   }
}

void Inbox::replace(const InboxItem& item)
{
   auto pos = find_by_id(mItems, item.id);
   if(pos != mItems.end())
   {
       *pos = item;

       mDb.store(item.id, toJson(item));
   }
}

Id Inbox::add(const InboxItem& item)
{
   auto pos = find_by_id(mItems, item.id);
   if(pos == mItems.end())
   {
       auto newItem = item;
       newItem.id = Id::generate();

       mDb.store(newItem.id, toJson(item));

       return newItem.id;
   }

   return Id::Invalid;
}

}