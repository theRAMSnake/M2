#pragma once

#include <string>
#include <memory>

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

    virtual void onNewDay() = 0;
    virtual void onNewWeek() = 0;

    virtual ~ICore3(){}
};

std::shared_ptr<ICore3> createCore(const CoreConfig& config);

}