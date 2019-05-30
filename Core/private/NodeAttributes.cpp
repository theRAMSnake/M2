#include "../NodeAttributes.hpp"

namespace materia
{

NodeAttributes::NodeAttributes(const std::map<NodeAttributeType, std::string>& init)
: mStorage(init)
{

}

NodeAttributes::NodeAttributes()
{

}

bool NodeAttributes::contains(const NodeAttributeType attrType) const
{
    return mStorage.find(attrType) != mStorage.end();
}

NodeAttributes& NodeAttributes::operator= (const NodeAttributes& other)
{
    mStorage = other.mStorage;
    return *this;
} 

std::map<NodeAttributeType, std::string> NodeAttributes::toMap() const
{
    return mStorage;
}

}