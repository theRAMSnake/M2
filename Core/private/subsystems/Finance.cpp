#include "Finance.hpp"
#include "../ObjectManager.hpp"
#include "../Commands.hpp"
#include "../ExceptionsUtil.hpp"
#include "../rng.hpp"
#include <map>
#include <chrono>
#include <iostream>
#include <boost/date_time/gregorian/greg_date.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "../types/SimpleList.hpp"

namespace materia
{

boost::gregorian::date alignToStartOfMonth(const boost::gregorian::date& date)
{
   return boost::gregorian::date(date.year(), date.month(), 1);
}

bool isWithinLastYear(Object& event)
{
   auto val = event["timestamp"].get<Type::Timestamp>().value;
   return val > std::chrono::system_clock::to_time_t(std::chrono::system_clock::now() - std::chrono::hours(8760)) &&
      val < std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
}

boost::gregorian::date getMonthAlignment(Object& event)
{
   return alignToStartOfMonth(boost::posix_time::from_time_t(event["timestamp"].get<Type::Timestamp>().value).date());
}

std::string getDateStr(const boost::gregorian::date src)
{
   const std::locale fmt(std::locale::classic(), new boost::gregorian::date_facet("%m/%Y"));

   std::ostringstream os;
   os.imbue(fmt);
   os << src;

   return os.str();
}

Money toEur(const Money& src, const std::vector<Object>& currencies)
{
    Money result = src;

    if(result.currency != "EUR")
    {
        auto pos = std::find_if(currencies.begin(), currencies.end(), [&src](const Object& x){
                auto str = x["name"].get<Type::String>();
                return str == src.currency;
            });

        if(pos == currencies.end())
        {
            throw std::runtime_error("Unknown currency: " + src.currency);
        }

        auto mod = (*pos)["conversionRateToEur"].get<Type::Double>();
        result /= mod;
        result.currency = "EUR";
    }

    return result;
}

void FinanceSS::performFinancialAnalisys(const boost::gregorian::date& newDate)
{
   Money grandTotal;
   Money totalEarnings;
   Money totalSpendings;
   std::map<Id, std::map<boost::gregorian::date, Money>> amountByCategory;
   std::map<boost::gregorian::date, Money> total_per_month;
   std::map<boost::gregorian::date, int> months;

   auto date = alignToStartOfMonth(newDate);

   for(int i = 0; i < 12; i++)
   {
      months[date] = i;
      date -= boost::gregorian::months(1);
   }

   auto categories = mOm.getAll("finance_category");
   auto events = mOm.getAll("finance_event");
   auto currencies = mOm.getAll("currency");

   for(auto& ev : events)
   {
      if(isWithinLastYear(ev))
      {
         auto month = getMonthAlignment(ev);
         auto amount = toEur(ev["value"].get<Type::Money_v2>(), currencies);
         auto catId = ev["categoryId"].toId();
         if(ev["typeChoice"].get<Type::Choice>() == "Spending")
         {
            amountByCategory[catId][month] -= amount;
            grandTotal -= amount;
            total_per_month[month] -= amount;
            totalSpendings += amount;
         }
         else
         {
            amountByCategory[catId][month] += amount;
            grandTotal += amount;
            total_per_month[month] += amount;
            totalEarnings += amount;
         }
      }
      else
      {
         mOm.destroy(ev.getId());
      }
   }

   if(events.empty())
   {
      return;
   }

   auto balance = totalEarnings - totalSpendings;
   auto ratio = totalEarnings / totalSpendings;
   std::string status;

   if(ratio > 1.5)
   {
      mReward.setGenerator(Id("mod.finance"), "Excellent finance", 3, "Blue");
      status = "Excellent";
   }
   else if(ratio > 1.2)
   {
      mReward.setGenerator(Id("mod.finance"), "Great finance", 2, "Blue");
      status = "Great";
   }
   else if(ratio > 1.1)
   {
      mReward.setGenerator(Id("mod.finance"), "Good finance", 1, "Blue");
      status = "Good";
   }
   else if(ratio > 1)
   {
      mReward.removeGenerator(Id("mod.finance"));
      status = "Ok";
   }
   else
   {
      status = "Critical";
      auto p = balance.base / 10000.0; // 1% per 100 EUR
      mReward.setGenerator(Id("mod.finance"), "Bad finance", static_cast<int>(p * 10 - 1), "Blue");
   }

   //Compile report
   auto obj = mOm.getOrCreate(Id("financial_report"), "object");
   obj.clear();
   (obj)["balance"] = balance;
   (obj)["status"] = status;

   for(auto d : amountByCategory)
   {
      Object curCatBreakdown({"object"}, Id::generate());
      auto catPos = find_by_id(categories, d.first);
      auto catName = catPos == categories.end() ? d.first.getGuid() : (*catPos)["name"].get<Type::String>();

      curCatBreakdown["name"] = catName;

      Money total;
      for(auto m : d.second)
      {
         total += m.second;
         curCatBreakdown[getDateStr(m.first)] = m.second;
      }

      curCatBreakdown["total"] = total;

      obj.setChild(catName, curCatBreakdown);
   }

   Object totalPerMonth({"object"}, Id::generate());

   for(auto m : total_per_month)
   {
      totalPerMonth[getDateStr(m.first)] = m.second;
   }

   obj.setChild("totalPerMonth", totalPerMonth);

   mOm.modify(obj);
}

FinanceSS::FinanceSS(ObjectManager& om, RewardSS& reward, CommonSS& common)
: mOm(om)
, mReward(reward)
, mCommon(common)
{

}

void FinanceSS::onNewDay(const boost::gregorian::date& date)
{
   performFinancialAnalisys(date);
}

void FinanceSS::onNewWeek()
{
    
}

std::vector<TypeDef> FinanceSS::getTypes()
{
    std::vector<TypeDef> result;

    result.push_back({"finance_category", "finance_categories", {{"name", Type::String}}});
    result.push_back({"currency", "currencies", {{"name", Type::String}}});
    result.push_back({"finance_stock", "stocks", {
        {"ticker", Type::String},
        {"domain", Type::String},
        {"amount", Type::Int},
        {"lastKnownPrice", Type::Money_v2},
        {"isTradeRestricted", Type::Bool}
        }});
    result.push_back({"finance_dataPoint", "finance_dataPoints", {
        {"timestamp", Type::Timestamp},
        {"totalPortfolioValue", Type::Money_v2}
        }});
    result.push_back({"finance_investmentAction", "finance_investmentAction", {
        {"timestamp", Type::Timestamp},
        {"value", Type::Money_v2}
        }});
    result.push_back({"finance_event", "finance_events", {
        {"categoryId", Type::Reference, {}, "finance_category"},
        {"typeChoice", Type::Choice, {"Spending", "Earning"}},
        {"details", Type::String},
        {"value", Type::Money_v2},
        {"timestamp", Type::Timestamp}
        }});

    return result;
}

std::vector<CommandDef> FinanceSS::getCommandDefs()
{
    return {};
}

}

