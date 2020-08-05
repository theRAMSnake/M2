#pragma once
#include <vector>
#include "TypeSystem.hpp"

namespace materia
{

class ISubsystem
{
public:
    virtual void onNewDay() = 0;
    virtual void onNewWeek() = 0;
    virtual std::vector<TypeDef> getTypes() = 0;

    virtual ~ISubsystem(){}
};

}