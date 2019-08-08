#pragma once

#include "../IFinance.hpp"
#include "Database.hpp"

namespace materia
{

class Database;
class Finance : public IFinance
{
public:
   Finance(Database& db);

   std::vector<SpendingCategory> getCategories() const override;

   void removeCategory(const Id& id) override;
   Id addCategory(const SpendingCategory& item) override;
   void replaceCategory(const SpendingCategory& item) override;

   void removeSpendingEvent(const Id& id) override;
   Id addSpendingEvent(const SpendingEvent& item) override;
   void replaceSpendingEvent(const SpendingEvent& item) override;

   std::vector<SpendingEvent> queryEvents(const std::time_t from, const std::time_t to) const override;
   
private:
   std::unique_ptr<DatabaseTable> mCategoriesStorage;
   std::unique_ptr<DatabaseTable> mEventsStorage;
};

}