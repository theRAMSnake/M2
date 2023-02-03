#pragma once
#include "ObjectManager.hpp"

namespace materia
{

class EmptyValueProvider : public IValueProvider
{
public:
    void populate(Object& obj) const override;
};

}
