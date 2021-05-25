#pragma once

#include <ctime>
#include <set>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

std::time_t day_to_time_t(const decltype(boost::gregorian::day_clock::local_day())& src);

template< class T, class Pred>
void erase_if(std::vector<T>& c, Pred pred)
{
   c.erase(std::remove_if(c.begin(), c.end(), pred), c.end());
}

template< class T, class Y>
bool contains(const std::vector<T>& c, const Y& item)
{
   return std::find(c.begin(), c.end(), item) != c.end();
}

template<class T>
std::set<T> toSet(const std::vector<T>& vec)
{
   std::set<T> result;
   std::copy(vec.begin(), vec.end(), std::inserter(result, result.end()));
   return result;
}

template<class T>
std::size_t calcNumIdentical(const std::set<T>& a, const std::set<T>& b)
{
   std::size_t result = 0;

   for(auto iter = a.begin(); iter != a.end(); ++iter)
   {
      if(b.find(*iter) != b.end())
      {
         result++;
      }
   }

   return result;
}