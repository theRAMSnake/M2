#pragma once

#include <ctime>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

std::time_t day_to_time_t(const decltype(boost::gregorian::day_clock::local_day())& src);