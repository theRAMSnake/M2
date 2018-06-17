#include "Utils.hpp"

std::time_t day_to_time_t(const decltype(boost::gregorian::day_clock::local_day())& src)
{
   return boost::posix_time::to_time_t(boost::posix_time::ptime(src, boost::posix_time::time_duration(0, 0, 0)));
}