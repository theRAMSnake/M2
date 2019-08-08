#pragma once
#include <vector>
#include <ctime>
#include <Common/Id.hpp>

namespace materia
{

struct SpendingCategory
{
   Id id;
   std::string name;
};

struct SpendingEvent
{
   Id eventId;
   Id categoryId;

   std::string details;
   unsigned int amountEuroCents;
   std::time_t timestamp;
};

class IFinance
{
public:
    virtual std::vector<SpendingCategory> getCategories() const = 0;

    virtual void removeCategory(const Id& id) = 0;
    virtual Id addCategory(const SpendingCategory& item) = 0;
    virtual void replaceCategory(const SpendingCategory& item) = 0;

    virtual void removeSpendingEvent(const Id& id) = 0;
    virtual Id addSpendingEvent(const SpendingEvent& item) = 0;
    virtual void replaceSpendingEvent(const SpendingEvent& item) = 0;

    virtual std::vector<SpendingEvent> queryEvents(const std::time_t from, const std::time_t to) const = 0;

    virtual ~IFinance(){}
};

}