#pragma once

#include "../IFinance.hpp"
#include "Database.hpp"

namespace materia
{

class Database;
class IReward;
class Finance : public IFinance
{
public:
   Finance(Database& db);

   std::vector<FinanceCategory> getCategories() const override;

   void removeCategory(const Id& id) override;
   Id addCategory(const FinanceCategory& item) override;
   void replaceCategory(const FinanceCategory& item) override;

   void removeEvent(const Id& id) override;
   Id addEvent(const FinanceEvent& item) override;
   void replaceEvent(const FinanceEvent& item) override;

   std::vector<FinanceEvent> queryEvents(const std::time_t from, const std::time_t to) const override;
   FinanceReport getReport() const override;

   void performAnalisys(IReward& reward);
   
private:
   void saveReport(const FinanceReport& r);

   std::unique_ptr<DatabaseTable> mCategoriesStorage;
   std::unique_ptr<DatabaseTable> mEventsStorage;
   std::unique_ptr<DatabaseTable> mReportsStorage;
};

}