#pragma once
#include "Common/Id.hpp"
#include "../Object.hpp"

namespace materia
{

class ObjectManager;

namespace types
{

class Variable
{
public:
    Variable(ObjectManager& om, const Id& id);
    ~Variable();
    void operator =(const std::string& value);

private:
    bool mChanged = false;
    ObjectPtr mObject;
    ObjectManager& mOm;
};

}
}