#include "Inbox.hpp"

namespace materia
{

Inbox::Inbox(Database& db)
: mStorage(db.getTable("inbox"))
{
    mStorage->foreach([&](std::string id, std::string json) 
    {
        mItems.push_back({id, json});
    });
}

std::vector<InboxItem> Inbox::get() const
{
   return mItems;
}

void Inbox::remove(const Id& id)
{
   auto pos = find_by_id(mItems, id);
   if(pos != mItems.end())
   {
       mItems.erase(pos);
       mStorage->erase(id);
   }
}

void Inbox::replace(const InboxItem& item)
{
   auto pos = find_by_id(mItems, item.id);
   if(pos != mItems.end())
   {
       *pos = item;

       mStorage->store(item.id, item.text);
   }
}

Id Inbox::add(const InboxItem& item)
{
   auto pos = find_by_id(mItems, item.id);
   if(pos == mItems.end())
   {
       auto newItem = item;
       newItem.id = Id::generate();

       mItems.push_back(newItem);
       mStorage->store(newItem.id, item.text);

       return newItem.id;
   }

   return Id::Invalid;
}

}