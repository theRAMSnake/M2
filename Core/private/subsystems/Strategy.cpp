#include "Strategy.hpp"
#include "../ObjectManager.hpp"

namespace materia
{

StrategySS::StrategySS(ObjectManager& objMan, RewardSS& reward)
: mOm(objMan)
, mReward(reward)
{

}

void StrategySS::onNewDay()
{
   for(auto o : mOm.getAll("strategy_node"))
   {
       if((o)["type"].get<Type::Option>() == 4 && (o)["date"].get<Type::Timestamp>().value < time(0))
       {
           (o)["isAchieved"] = true;
           mOm.modify(o);
       }
   }
}

void StrategySS::onNewWeek()
{
    
}

std::vector<std::string> allowedTypes = {"Goal", "Task", "Counter", "Watch", "Wait"};

std::vector<TypeDef> StrategySS::getTypes()
{
    std::vector<TypeDef> result;

    result.push_back({"strategy_node", "strategy_nodes", {
        {"title", Type::String},
        {"details", Type::String},
        {"type", Type::Option, allowedTypes},
        {"isAchieved", Type::Bool},
        {"parentNodeId", Type::String},
        {"date", Type::Timestamp},
        {"value", Type::Int},
        {"target", Type::Int},
        {"reward", Type::Int}
        }});
    result.back().handlers.onBeforeDelete = std::bind(&StrategySS::handleNodeBeforeDelete, this, std::placeholders::_1);
    result.back().handlers.onChanging = std::bind(&StrategySS::handleNodeChanging, this, std::placeholders::_1, std::placeholders::_2);
    result.back().handlers.onValidation = std::bind(&StrategySS::validateNode, this, std::placeholders::_1);

    result.push_back({"strategy_link", "strategy_links", {
        {"idFrom", Type::String},
        {"idTo", Type::String}
        }});
    result.back().handlers.onValidation = std::bind(&StrategySS::validateLink, this, std::placeholders::_1);

    return result;
}

std::vector<CommandDef> StrategySS::getCommandDefs()
{
    return {};
}

/*
    IsAchieved:
    Goal, Task, Watch - isDone
    Counter - value >= target
    Wait - time based
*/

void StrategySS::handleNodeBeforeDelete(Object& obj)
{
    //Delete all children
    for(auto o : mOm.getAll("strategy_node"))
    {
        if((o)["parentNodeId"].toId() == obj.getId())
        {
            mOm.destroy(o.getId());
        }
    }

    //Break all links
    for(auto& l : mOm.getAll("strategy_link"))
    {
        if((l)["idFrom"].toId() == obj.getId() ||
            (l)["idTo"].toId() == obj.getId())
        {
            mOm.destroy(l.getId());
        }
    }
}

void StrategySS::handleNodeChanging(const Object& before, Object& after)
{
    if(after["type"].get<Type::Option>() == 2 && after["value"].get<Type::Int>() >= 
        after["target"].get<Type::Int>())
    {
        after["isAchieved"] = true;
    }

    if(before["isAchieved"].get<Type::Bool>() == false &&
        after["isAchieved"].get<Type::Bool>() == true)
    {
        std::cout << "+";
        mReward.addPoints(after["reward"].get<Type::Int>());
    }
}

void StrategySS::validateNode(Object& obj)
{
    //Check type is in the range
    auto opt = obj["type"].get<Type::Option>();
    if(opt < 0 || opt >= static_cast<int>(allowedTypes.size()))
    {
        throw std::runtime_error("Node validation failed: unsupported type");
    }

    //Make sure no parent/children loops exist
    if(obj["parentNodeId"].toId() == obj.getId())
    {
        throw std::runtime_error("Node validation failed: parentNodeId cannot be self");
    }

    //Check parent exist
    auto parentId = obj["parentNodeId"].toId();
    if(parentId != Id::Invalid)
    {
        try
        {
            auto p = mOm.get(parentId);

            //Go up, Achieve root faster than self
            while((p)["parentNodeId"].toId() != Id::Invalid)
            {
                p = mOm.get((p)["parentNodeId"].toId());
                if(p.getId() == obj.getId())
                {
                    throw std::runtime_error("Node validation failed: parent children relations forms loop");
                }
            }
        }
        catch(...)
        {
            throw std::runtime_error("Node validation failed: parent does not exist");
        }
    }
}

void StrategySS::validateLink(Object& obj)
{
    //Make sure link is unique
    for(auto& l : mOm.getAll("strategy_link"))
    {
        if((l)["idFrom"].toId() == obj["idFrom"].toId() &&
            (l)["idTo"].toId() == obj["idTo"].toId())
        {
            throw std::runtime_error("Link validation failed: not unique");
        }
    }

    //Make sure from/to are different
    if(obj["idFrom"].toId() == obj["idTo"].toId())
    {
        throw std::runtime_error("Link validation failed: from = to");
    }

    //Check both nodes exist
    try
    {
        mOm.get(obj["idFrom"].toId());
        mOm.get(obj["idTo"].toId());
    }
    catch(...)
    {
        throw std::runtime_error("Link validation failed: refered node does not exist");
    }

    //Make sure no loops exist => Go forward until same node is reached or endpoint is reached
    std::set<Id> visitedLinks;
    
    Object next(obj);
    visitedLinks.insert(next.getId());
    bool nextFound = true;
    while(nextFound)
    {
        nextFound = false;
        for(auto& l : mOm.getAll("strategy_link"))
        {
            if((l)["idFrom"].toId() == (next)["idTo"].toId())
            {
                if(visitedLinks.contains(l.getId()))
                {
                    throw std::runtime_error("Link validation failed: graph forms loop");
                }

                nextFound = true;
                visitedLinks.insert(l.getId());
                next = l;
                break;
            }
        }
    }
}

void StrategySS::onCalendarReferenceCompleted(const Id& id)
{
    try
    {
        Object obj = mOm.get(id);
        auto tp = obj["type"].get<Type::Option>();
        //If node goal/task -> change isAchieved
        if(tp == 0 || tp == 1)
        {
            obj["isAchieved"] = true;
        }
        //If counter -> ++
        else if(tp == 2)
        {
            obj["value"] = obj["value"].get<Type::Int>() + 1;
        }

        mOm.modify(obj);   
    }
    catch(...)
    {
        //No problem - reference is weak
    }
}

}
