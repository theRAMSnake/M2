#pragma once

#include "ICore.hpp"

namespace materia
{

class ICore3 : public ICore
{
public:
    virtual std::string executeCommandJson(const std::string& json) = 0;

    virtual ~ICore3(){}
};

std::shared_ptr<ICore3> createCore(const CoreConfig& config);

}