#include "Finance.hpp"
#include "JsonSerializer.hpp"

BIND_JSON2(materia::FinanceCategory, id, name)

SERIALIZE_AS_INTEGER(materia::EventType)
BIND_JSON6(materia::FinanceEvent, eventId, categoryId, type, details, amountEuroCents, timestamp)

namespace materia
{

Finance::Finance(Database& db)
: mCategoriesStorage(db.getTable("finance_categories"))
, mEventsStorage(db.getTable("finance_events"))
{

}

std::vector<FinanceCategory> Finance::getCategories() const
{
   std::vector<FinanceCategory> result;

   mCategoriesStorage->foreach([&](std::string id, std::string json) 
   {
      result.push_back(readJson<FinanceCategory>(json));
   });

   return result;
}

void Finance::removeCategory(const Id& id)
{
   mCategoriesStorage->erase(id);
}

Id Finance::addCategory(const FinanceCategory& item)
{
   auto newItem = item;
   newItem.id = Id::generate();

   std::string json = writeJson(newItem);
   mCategoriesStorage->store(newItem.id, json);

   return newItem.id;
}

void Finance::replaceCategory(const FinanceCategory& item)
{
   std::string json = writeJson(item);
   mCategoriesStorage->store(item.id, json);
}

void Finance::removeEvent(const Id& id)
{
   mEventsStorage->erase(id);
}

Id Finance::addEvent(const FinanceEvent& item)
{
   auto newItem = item;
   newItem.eventId = Id::generate();

   std::string json = writeJson(newItem);
   mEventsStorage->store(newItem.eventId, json);

   return newItem.eventId;
}

void Finance::replaceEvent(const FinanceEvent& item)
{
   std::string json = writeJson(item);
   mEventsStorage->store(item.eventId, json);
}

std::vector<FinanceEvent> Finance::queryEvents(const std::time_t from, const std::time_t to) const
{
   std::vector<FinanceEvent> result;

   mEventsStorage->foreach([&](std::string id, std::string json) 
   {
      auto item = readJson<FinanceEvent>(json);
      if(item.timestamp <= to && item.timestamp >= from)
      {
         result.push_back(item);
      }
   });

   return result;
}

}