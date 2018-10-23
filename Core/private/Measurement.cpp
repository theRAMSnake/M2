#include "Measurement.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

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
    boost::property_tree::ptree pt;
    std::istringstream is (json);
    read_json (is, pt);
    
    mImpl.id = pt.get<std::string> ("id");
    mImpl.name = pt.get<std::string> ("name");
    mImpl.value = pt.get<TValue> ("value");
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
    boost::property_tree::ptree pt;

    pt.put ("id", mImpl.id.getGuid());
    pt.put ("name", mImpl.name);
    pt.put ("value", mImpl.value);

    std::ostringstream buf; 
    write_json (buf, pt, false);
    return buf.str();
}

Id Measurement::getId() const
{
    return mImpl.id;
}

}
}