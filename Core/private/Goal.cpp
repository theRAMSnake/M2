#include "Goal.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

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
    boost::property_tree::ptree pt;
    std::istringstream is (json);
    read_json (is, pt);
    
    mImpl.id = pt.get<std::string> ("id");
    mImpl.name = pt.get<std::string> ("name");
    mImpl.notes = pt.get<std::string> ("notes");
    mImpl.focused = pt.get<bool> ("focused");
    mImpl.achieved = pt.get<bool> ("achieved");
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

std::string Goal::toJson() const
{
    boost::property_tree::ptree pt;

    pt.put ("id", mImpl.id.getGuid());
    pt.put ("name", mImpl.name);
    pt.put ("notes", mImpl.notes);
    pt.put ("focused", mImpl.focused);
    pt.put ("achieved", mImpl.achieved);

    std::ostringstream buf; 
    write_json (buf, pt, false);
    return buf.str();
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

}
}