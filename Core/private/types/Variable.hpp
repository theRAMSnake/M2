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
    void operator =(const int value);
    void inc(const int value);
    bool operator >(const int x) const;

private:
    bool mChanged = false;
    Object mObject;
    ObjectManager& mOm;
};

}
}