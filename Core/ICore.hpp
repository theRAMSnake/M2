#pragma once

#include <memory>

namespace materia
{

class IStrategy;
class IStrategy_v2;
class IBackuper;

struct CoreConfig
{
    std::string dbFileName;
};

class ICore
{
public:
    virtual IStrategy_v2& getStrategy_v2() = 0;
    virtual IBackuper& getBackuper() = 0;

    virtual void onNewDay() = 0;
    virtual void onNewWeek() = 0;

    virtual ~ICore(){}
};

}