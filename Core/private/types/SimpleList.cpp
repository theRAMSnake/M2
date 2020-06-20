#include "SimpleList.hpp"
#include "../EmptyValueProvider.hpp"
#include "../ObjectManager.hpp"

namespace materia
{
namespace types
{

SimpleList::SimpleList(ObjectManager& om, const Id& id)
: mObject(om.getOrCreate(id, "simple_list"))
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

std::string SimpleList::at(const std::size_t pos)
{
    auto v = static_cast<std::vector<std::string>>((*mObject)["objects"]);
    return v[pos];
}

}
}