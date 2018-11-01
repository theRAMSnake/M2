#include "Goal.hpp"
#include "JsonSerializer.hpp"

BIND_JSON5(materia::Goal, id, name, notes, focused, achieved)

namespace materia
{
namespace strategy
{

Goal::Goal(const materia::Goal& props)
{
    mImpl.achieved = false;
    accept(props);
}

Goal::Goal(const std::string& json)
{
    mImpl = readJson<materia::Goal>(json);
}

void Goal::accept(const materia::Goal& props)
{
    mImpl = props;

    updateAchieved();

    OnChanged(*this);
}

const materia::Goal& Goal::getProps() const
{
    return mImpl;
}

void Goal::connect(const std::shared_ptr<Objective>& obj)
{
    auto con = new ConnectedObject<std::shared_ptr<Objective>, boost::signals2::connection>(
        obj,
        obj->OnReachedChanged.connect(std::bind(&Goal::OnObjReachedChanged, this))
        );

    mObjectives.insert(std::make_pair(obj->getProps().id, con));
    
    UpdateAndSaveAchieved();
}

void Goal::OnObjReachedChanged()
{
    UpdateAndSaveAchieved();
}

void Goal::disconnect(const std::shared_ptr<Objective>& obj)
{
    mObjectives.erase(obj->getProps().id);

    UpdateAndSaveAchieved();
}

std::vector<Id> Goal::getObjectives()
{
    decltype(getObjectives()) result;

    for(auto x : mObjectives)
    {
        result.push_back(x.second->get()->getProps().id);
    }

    return result;  
}

void Goal::UpdateAndSaveAchieved()
{
    if(updateAchieved())
    {
        OnChanged(*this);
    }
}

bool Goal::updateAchieved()
{
    bool newAchieved = calculateAchieved();
    if(mImpl.achieved != newAchieved)
    {
        mImpl.achieved = newAchieved;
        OnAchievedChanged();
        return true;
    }

    return false;
}

bool Goal::calculateAchieved()
{
    bool result = false;

    if(!mObjectives.empty())
    {
        result = true;
        for(auto o : mObjectives)
        {
            result = result && o.second->get()->getProps().reached;
            if(!result)
            {
                return false;
            }
        }
    }

    return result;
}

Id Goal::getId() const
{
    return mImpl.id;
}

std::string Goal::toJson() const
{
    return writeJson(mImpl);
}

}
}