#include "Objective.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace materia
{
namespace strategy
{

Objective::Objective(const materia::Objective& props)
{
    accept(props);
}

Objective::Objective(const std::string& json)
{
    boost::property_tree::ptree pt;
    std::istringstream is (json);
    read_json (is, pt);
    
    mImpl.id = pt.get<std::string> ("id");
    mImpl.parentGoalId = pt.get<std::string> ("parent_goal_id");
    mImpl.name = pt.get<std::string> ("name");
    mImpl.notes = pt.get<std::string> ("notes");
    mImpl.measurementId = pt.get<std::string> ("meas_id");
    mImpl.expectedMeasurementValue = pt.get<Measurement::TValue> ("expected_meas_value");
    mImpl.reached = pt.get<bool> ("reached");
}

void Objective::accept(const materia::Objective& props)
{
    bool oldReached = mImpl.reached;
    mImpl = props;

    updateReached(oldReached);

    OnChanged(*this);
}

const materia::Objective& Objective::getProps() const
{
    return mImpl;
}

void Objective::connect(Measurement& meas)
{
    mMeasConnection.disconnect();
    mMeasConnection = meas.OnValueChanged.connect(std::bind(&Objective::OnMeasValueChanged, this, std::placeholders::_1));
    mLastKnowMeasValue = meas.getProps().value;
    if(updateReached(mImpl.reached))
    {
        OnChanged(*this);
    }
}

void Objective::disconnect(const Measurement& meas)
{
    mImpl.measurementId = Id::Invalid;
    mMeasConnection.disconnect();
}

Objective::~Objective()
{
    mMeasConnection.disconnect();
}

std::string Objective::toJson() const
{
    boost::property_tree::ptree pt;

    pt.put ("id", mImpl.id.getGuid());
    pt.put ("parent_goal_id", mImpl.parentGoalId.getGuid());
    pt.put ("name", mImpl.name);
    pt.put ("notes", mImpl.notes);
    pt.put ("meas_id", mImpl.measurementId.getGuid());
    pt.put ("expected_meas_value", mImpl.expectedMeasurementValue);
    pt.put ("reached", mImpl.reached);

    std::ostringstream buf; 
    write_json (buf, pt, false);
    return buf.str();
}

bool Objective::updateReached(const bool oldReached)
{
    if(mImpl.measurementId != Id::Invalid)
    {
        mImpl.reached = mLastKnowMeasValue >= mImpl.expectedMeasurementValue;
    }
    
    if(mImpl.reached != oldReached)
    {
        OnReachedChanged(mImpl.reached);
    }

    return mImpl.reached != oldReached;
}

void Objective::OnMeasValueChanged(const Measurement::TValue value)
{
    mLastKnowMeasValue = value;
    if(updateReached(mImpl.reached))
    {
        OnChanged(*this);
    }
}

Id Objective::getId() const
{
    return mImpl.id;
}

}
}