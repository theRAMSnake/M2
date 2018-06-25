#pragma once

std::time_t WtDateTimeToTimestamp(const Wt::WDate& date, const Wt::WTime& time);
std::time_t WtDateToTimeStamp(const Wt::WDate& date);

Wt::WDate timestampToWtDate(const std::time_t& timestamp);
Wt::WTime timestampToWtTime(const std::time_t& timestamp);