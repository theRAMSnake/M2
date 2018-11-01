#include "Measurement.hpp"
#include "JsonSerializer.hpp"

BIND_JSON3(materia::Measurement, id, name, value)

namespace materia
{
namespace strategy
{

Measurement::Measurement(const materia::Measurement& props)
{
    accept(props);
}

Measurement::Measurement(const std::string& json)
{
    mImpl = readJson<materia::Measurement>(json);
}

void Measurement::accept(const materia::Measurement& props)
{
    if(mImpl.value != props.value)
    {
        OnValueChanged(props.value);
    }
    
    mImpl = props;

    OnChanged(*this);
}

const materia::Measurement& Measurement::getProps() const
{
    return mImpl;
}

std::string Measurement::toJson() const
{
    return writeJson(mImpl);
}

Id Measurement::getId() const
{
    return mImpl.id;
}

}
}