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

Variable::~Variable()
{
    if(mChanged)
    {
        mOm.modify(mObject);
    }
}

}
}