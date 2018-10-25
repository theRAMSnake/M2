#include "WtConverters.hpp"
#include <Wt/WTime.h>
#include <boost/date_time/posix_time/posix_time.hpp>

std::time_t WtDateTimeToTimestamp(const Wt::WDate& date, const Wt::WTime& time)
{
    return boost::posix_time::to_time_t(boost::posix_time::ptime(
        boost::gregorian::date(
            date.year(), 
            static_cast<boost::date_time::months_of_year>(date.month()), 
            date.day()),
            boost::posix_time::hours(time.hour()) +
            boost::posix_time::minutes(time.minute())));
}

std::time_t WtDateToTimeStamp(const Wt::WDate& date)
{
    return boost::posix_time::to_time_t(boost::posix_time::ptime(
        boost::gregorian::date(date.year(), static_cast<boost::date_time::months_of_year>(date.month()), date.day())));
}

Wt::WDate timestampToWtDate(const std::time_t& timestamp)
{
    return Wt::WDate(std::chrono::system_clock::from_time_t(timestamp));
}

Wt::WTime timestampToWtTime(const std::time_t& timestamp)
{
    return Wt::WDateTime(std::chrono::system_clock::from_time_t(timestamp)).time();
}
