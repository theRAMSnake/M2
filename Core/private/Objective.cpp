#include "Objective.hpp"
#include "JsonSerializer.hpp"

BIND_JSON7(materia::Objective, id, parentGoalId, name, notes, resourceId, expectedResourceValue, reached)

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
    mImpl = readJson<materia::Objective>(json);
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

void Objective::connect(Resource& res)
{
    mMeasConnection.disconnect();
    mMeasConnection = res.OnValueChanged.connect(std::bind(&Objective::OnMeasValueChanged, this, std::placeholders::_1));
    mLastKnowMeasValue = res.getProps().value;
    if(updateReached(mImpl.reached))
    {
        OnChanged(*this);
    }
}

void Objective::disconnect(const Resource& res)
{
    mImpl.resourceId = Id::Invalid;
    mMeasConnection.disconnect();
}

Objective::~Objective()
{
    mMeasConnection.disconnect();
}

std::string Objective::toJson() const
{
    return writeJson(mImpl);
}

bool Objective::updateReached(const bool oldReached)
{
    if(mImpl.resourceId != Id::Invalid)
    {
        mImpl.reached = mLastKnowMeasValue >= mImpl.expectedResourceValue;
    }
    
    if(mImpl.reached != oldReached)
    {
        OnReachedChanged(mImpl.reached);
    }

    return mImpl.reached != oldReached;
}

void Objective::OnMeasValueChanged(const Resource::TValue value)
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