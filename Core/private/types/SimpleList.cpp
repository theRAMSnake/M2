#include "SimpleList.hpp"
#include "../ObjectManager.hpp"

namespace materia
{
namespace types
{

SimpleList::SimpleList(ObjectManager& om, const Id& id)
: mObject(om.get(id))
, mOm(om)
{

}

void SimpleList::add(const std::string& item)
{
    auto v = static_cast<std::vector<std::string>>((*mObject)["objects"]);
    v.push_back(item);
    (*mObject)["objects"] = v;
    mChanged = true;
}

std::size_t SimpleList::size()
{
    auto v = static_cast<std::vector<std::string>>((*mObject)["objects"]);
    return v.size();
} 

SimpleList::~SimpleList()
{
    if(mChanged)
    {
        mOm.modify(*mObject);
    }
}

}
}