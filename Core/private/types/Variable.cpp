#include "Variable.hpp"
#include "../EmptyValueProvider.hpp"
#include "../ObjectManager.hpp"

namespace materia
{
namespace types
{

Variable::Variable(ObjectManager& om, const Id& id)
: mObject(om.getOrCreate(id, "variable"))
, mOm(om)
{
    
}

void Variable::operator =(const std::string& value)
{
    (mObject)["value"] = value;
    mChanged = true;
}

void Variable::operator =(const int value)
{
    (mObject)["value"] = value;
    mChanged = true;
}

void Variable::dec(const int value)
{
    inc(-value);
}

void Variable::inc(const int value)
{
    int oldVal = 0;

    try
    {
        oldVal = mObject["value"].get<Type::Int>();
    }
    catch(std::runtime_error&)
    {
        //Use 0 as default value
    }

    mObject["value"] = oldVal + value;
    mChanged = true;
}

int Variable::asInt() const
{
    return mObject["value"].get<Type::Int>();
}

bool Variable::operator> (const int x) const
{
    try
    {
        return mObject["value"].get<Type::Int>() > x;
    }
    catch(std::runtime_error&)
    {
        return false;
    }
}

bool Variable::operator< (const int x) const
{
    try
    {
        return mObject["value"].get<Type::Int>() < x;
    }
    catch(std::runtime_error&)
    {
        return false;
    }
}

Variable::~Variable()
{
    if(mChanged)
    {
        mOm.modify(mObject);
    }
}

}
}
