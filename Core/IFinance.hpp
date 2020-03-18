#pragma once
#include <vector>
#include <ctime>
#include <Common/Id.hpp>

namespace materia
{

struct FinanceCategory
{
   Id id;
   std::string name;
};

enum EventType
{
   Spending,
   Earning
};

struct FinanceEvent
{
   Id eventId;
   Id categoryId;

   EventType type = EventType::Spending;

   std::string details;
   unsigned int amountEuroCents;
   std::time_t timestamp;
};

class IFinance
{
public:
    virtual std::vector<FinanceCategory> getCategories() const = 0;

    virtual void removeCategory(const Id& id) = 0;
    virtual Id addCategory(const FinanceCategory& item) = 0;
    virtual void replaceCategory(const FinanceCategory& item) = 0;

    virtual void removeEvent(const Id& id) = 0;
    virtual Id addEvent(const FinanceEvent& item) = 0;
    virtual void replaceEvent(const FinanceEvent& item) = 0;

    virtual std::vector<FinanceEvent> queryEvents(const std::time_t from, const std::time_t to) const = 0;

    virtual ~IFinance(){}
};

}