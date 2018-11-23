#include "Resource.hpp"
#include "JsonSerializer.hpp"

BIND_JSON3(materia::Resource, id, name, value)

namespace materia
{
namespace strategy
{

Resource::Resource(const materia::Resource& props)
{
    accept(props);
}

Resource::Resource(const std::string& json)
{
    mImpl = readJson<materia::Resource>(json);
}

void Resource::accept(const materia::Resource& props)
{
    if(mImpl.value != props.value)
    {
        OnValueChanged(props.value);
    }
    
    mImpl = props;

    OnChanged(*this);
}

const materia::Resource& Resource::getProps() const
{
    return mImpl;
}

std::string Resource::toJson() const
{
    return writeJson(mImpl);
}

Id Resource::getId() const
{
    return mImpl.id;
}

}
}