#pragma once

#include <ctime>
#include <Wt/WDate.h>
#include <boost/date_time/gregorian/greg_date.hpp>

std::time_t WtDateTimeToTimestamp(const Wt::WDate& date, const Wt::WTime& time);
std::time_t WtDateToTimeStamp(const Wt::WDate& date);
boost::gregorian::date WtDateToGregorian(const Wt::WDate& date);

Wt::WDate timestampToWtDate(const std::time_t& timestamp);
Wt::WTime timestampToWtTime(const std::time_t& timestamp);
boost::gregorian::date timestampToGregorian(const std::time_t& timestamp);
Wt::WDate gregorianToWtDate(const boost::gregorian::date& src);

std::string currencyToString(const unsigned int cents);