#pragma once
#include "TypeHandler.hpp"

namespace materia
{

class EmptyValueProvider : public IValueProvider
{
public:
    void populate(Object& obj) const override;
};

}