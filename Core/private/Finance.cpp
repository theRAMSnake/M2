#include "Finance.hpp"
#include "JsonSerializer.hpp"
#include "../IReward.hpp"
#include "../IInbox.hpp"

#include <chrono>

BIND_JSON2(materia::FinanceCategory, id, name)

SERIALIZE_AS_INTEGER(materia::FinanceStatus)
BIND_JSON2(materia::FinanceReport, status, balance)

SERIALIZE_AS_INTEGER(materia::EventType)
BIND_JSON6(materia::FinanceEvent, eventId, categoryId, type, details, amountEuroCents, timestamp)

namespace materia
{

const materia::Id REPORT_ID = materia::Id("r1");

Finance::Finance(Database& db)
: mCategoriesStorage(db.getTable("finance_categories"))
, mEventsStorage(db.getTable("finance_events"))
, mReportsStorage(db.getTable("finance_reports"))
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

void Finance::performAnalisys(IReward& reward, IInbox& inbox)
{
   auto now = std::chrono::system_clock::now();
   auto events = queryEvents(std::chrono::system_clock::to_time_t(now - std::chrono::hours(8760)), std::chrono::system_clock::to_time_t(now));

   unsigned int totalEarnings = 0;
   unsigned int totalSpendings = 0;

   for(auto& e : events)
   {
      if(e.type == EventType::Earning)
      {
         totalEarnings += e.amountEuroCents;
      }
      else if(e.type == EventType::Spending)
      {
         totalSpendings += e.amountEuroCents;
      }
   }

   if(totalSpendings == 0)
   {
      return;
   }

   FinanceReport r;
   r.balance = static_cast<int>(totalEarnings) - static_cast<int>(totalSpendings);
   auto ratio = static_cast<double>(totalEarnings) / totalSpendings;
   if(ratio > 1.5)
   {
      reward.addPoints(3);
      r.status = FinanceStatus::Excellent;
   }
   else if(ratio > 1.2)
   {
      reward.addPoints(2);
      r.status = FinanceStatus::Great;
   }
   else if(ratio > 1.1)
   {
      r.status = FinanceStatus::Good;
      reward.addPoints(1);
   }
   else if(ratio > 1)
   {
      r.status = FinanceStatus::Ok;
   }
   else
   {
      r.status = FinanceStatus::Critical;
      auto p = (r.balance * -1) / 100000 * 10;
      reward.removePoints(3);

      if(p > rand() % 100)
      {
         inbox.add({Id::Invalid, "Work hard curse with p = " + std::to_string(p)});
      }
   }

   saveReport(r);
}

void Finance::saveReport(const FinanceReport& r)
{
   mReportsStorage->store(REPORT_ID, writeJson(r));
}

FinanceReport Finance::getReport() const
{
   FinanceReport result;
   mReportsStorage->foreach([&](std::string id, std::string json) 
   {
      result = readJson<FinanceReport>(json);
   });

   return result;
}

}