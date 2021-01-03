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

Variable::~Variable()
{
    if(mChanged)
    {
        mOm.modify(mObject);
    }
}

}
}