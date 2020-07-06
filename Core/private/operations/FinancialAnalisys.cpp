#include "FinancialAnalisys.hpp"
#include <map>
#include <chrono>
#include <iostream>
#include <boost/date_time/gregorian/greg_date.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "../ObjectManager.hpp"
#include "../types/SimpleList.hpp"
#include "../../IReward.hpp"

namespace materia
{

boost::gregorian::date alignToStartOfMonth(const boost::gregorian::date& date)
{
   return boost::gregorian::date(date.year(), date.month(), 1);
}

bool isWithinLastYear(Object& event)
{
   auto val = static_cast<Time>(event["timestamp"]).value ;
   return val > std::chrono::system_clock::to_time_t(std::chrono::system_clock::now() - std::chrono::hours(8760)) &&
      val < std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
}

boost::gregorian::date getMonthAlignment(Object& event)
{
   return alignToStartOfMonth(boost::posix_time::from_time_t(static_cast<Time>(event["timestamp"]).value).date());
}

std::string getDateStr(const boost::gregorian::date src)
{
   const std::locale fmt(std::locale::classic(), new boost::gregorian::date_facet("%m/%Y"));

   std::ostringstream os;
   os.imbue(fmt);
   os << src;

   return os.str();
}

void performFinancialAnalisys(ObjectManager& objMan, IReward& reward, types::SimpleList& inbox)
{
   int grandTotal = 0;
   unsigned int totalEarnings = 0;
   unsigned int totalSpendings = 1;
   std::map<Id, std::map<boost::gregorian::date, int>> amountByCategory;
   std::map<boost::gregorian::date, int> total_per_month;
   std::map<boost::gregorian::date, int> months;

   auto date = alignToStartOfMonth(boost::gregorian::date(boost::gregorian::day_clock::local_day()));

   for(int i = 0; i < 12; i++)
   {
      months[date] = i;
      date -= boost::gregorian::months(1);
   }

   std::vector<Id> toDelete;

   auto categories = objMan.getAll("finance_category");
   auto events = objMan.getAll("finance_event");

   for(auto e : events)
   {
      auto ev = *e;
      if(isWithinLastYear(ev))
      {
         auto month = getMonthAlignment(ev);
         auto amount = static_cast<int>(ev["amountEuroCents"]);
         auto catId = static_cast<Id>(ev["categoryId"]);
         if(static_cast<int>(ev["type"]) == 0/*spending*/)
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
         //toDelete.push_back(e["id"]);
      }
   }

   auto balance = static_cast<int>(totalEarnings) - static_cast<int>(totalSpendings);
   auto ratio = static_cast<double>(totalEarnings) / totalSpendings;
   std::string status;

   if(ratio > 1.5)
   {
      reward.addPoints(3);
      status = "Excellent";
   }
   else if(ratio > 1.2)
   {
      reward.addPoints(2);
      status = "Great";
   }
   else if(ratio > 1.1)
   {
      status = "Good";
      reward.addPoints(1);
   }
   else if(ratio > 1)
   {
      status = "Ok";
   }
   else
   {
      status = "Critical";
      auto p = (balance * -1) / 100000 * 10;
      reward.removePoints(3);

      if(p > rand() % 100)
      {
         inbox.add("Work hard curse with p = " + std::to_string(p));
      }
   }

   //Compile report
   auto obj = objMan.getOrCreate(Id("financial_report"), "object");
   obj->clear();
   (*obj)["balance"] = balance;
   (*obj)["status"] = status;

   for(auto d : amountByCategory)
   {
      Object curCatBreakdown({"object"}, Id::generate());
      auto catPos = find_by_id(categories, d.first);
      auto catName = catPos == categories.end() ? d.first.getGuid() : static_cast<std::string>((**catPos)["name"]);

      curCatBreakdown["name"] = catName;
      
      unsigned int total = 0;
      for(auto m : d.second)
      {
         total += m.second;
         curCatBreakdown[getDateStr(m.first)] = m.second;
      }

      curCatBreakdown["total"] = static_cast<int>(total);

      (*obj)[catName] = curCatBreakdown;
   }

   Object totalPerMonth({"object"}, Id::generate());

   for(auto m : total_per_month)
   {
      totalPerMonth[getDateStr(m.first)] = m.second;
   }

   (*obj)["totalPerMonth"] = totalPerMonth;

   objMan.modify(*obj);

   std::cout << obj->toJson();
}

}