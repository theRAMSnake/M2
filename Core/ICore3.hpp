#pragma once

#include <string>
#include <memory>
#include "boost/date_time/gregorian/gregorian_types.hpp"

namespace materia
{

struct CoreConfig
{
    std::string dbFileName;
};

class ICore3
{
public:
    virtual std::string executeCommandJson(const std::string& json) = 0;

    virtual void onNewDay(const boost::gregorian::date& date) = 0;
    virtual void onNewWeek() = 0;
    virtual void TEST_reinitReward() = 0;

    virtual ~ICore3(){}
};

std::shared_ptr<ICore3> createCore(const CoreConfig& config);

}
