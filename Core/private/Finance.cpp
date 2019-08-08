#include "Finance.hpp"
#include "JsonSerializer.hpp"

BIND_JSON2(materia::SpendingCategory, id, name)
BIND_JSON5(materia::SpendingEvent, eventId, categoryId, details, amountEuroCents, timestamp)

namespace materia
{

Finance::Finance(Database& db)
: mCategoriesStorage(db.getTable("finance_categories"))
, mEventsStorage(db.getTable("finance_events"))
{

}

std::vector<SpendingCategory> Finance::getCategories() const
{
   std::vector<SpendingCategory> result;

   mCategoriesStorage->foreach([&](std::string id, std::string json) 
   {
      result.push_back(readJson<SpendingCategory>(json));
   });

   return result;
}

void Finance::removeCategory(const Id& id)
{
   mCategoriesStorage->erase(id);
}

Id Finance::addCategory(const SpendingCategory& item)
{
   auto newItem = item;
   newItem.id = Id::generate();

   std::string json = writeJson(newItem);
   mCategoriesStorage->store(newItem.id, json);

   return newItem.id;
}

void Finance::replaceCategory(const SpendingCategory& item)
{
   std::string json = writeJson(item);
   mCategoriesStorage->store(item.id, json);
}

void Finance::removeSpendingEvent(const Id& id)
{
   mEventsStorage->erase(id);
}

Id Finance::addSpendingEvent(const SpendingEvent& item)
{
   auto newItem = item;
   newItem.eventId = Id::generate();

   std::string json = writeJson(newItem);
   mEventsStorage->store(newItem.eventId, json);

   return newItem.eventId;
}

void Finance::replaceSpendingEvent(const SpendingEvent& item)
{
   std::string json = writeJson(item);
   mEventsStorage->store(item.eventId, json);
}

std::vector<SpendingEvent> Finance::queryEvents(const std::time_t from, const std::time_t to) const
{
   std::vector<SpendingEvent> result;

   mEventsStorage->foreach([&](std::string id, std::string json) 
   {
      auto item = readJson<SpendingEvent>(json);
      if(item.timestamp <= to && item.timestamp >= from)
      {
         result.push_back(item);
      }
   });

   return result;
}

}