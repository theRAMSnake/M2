#include "Commands.hpp"

namespace materia
{

CreateCommand::CreateCommand(const std::vector<std::string>& traits, const Id id, const Params& params)
: mTraits(traits)
, mId(id)
, mParams(params)
{

}

ExecutionResult CreateCommand::execute(ObjectManager& objManager)
{
    return objManager.create(mTraits, mId, mParams);
}

ModifyCommand::ModifyCommand(const Id& id, const Params& params)
: mId(id)
, mParams(params)
{

}

ExecutionResult ModifyCommand::execute(ObjectManager& objManager)
{
    objManager.modify(mId, mParams);
    return Success{};
}

QueryCommand::QueryCommand(std::shared_ptr<Filter>& filter, const std::vector<Id>& ids)
: mIds(ids)
, mFilter(std::move(filter))
{

}

ExecutionResult QueryCommand::execute(ObjectManager& objManager)
{
    if(!mIds.empty())
    {
        return ObjectList{objManager.query(mIds)};
    }
    else if(static_cast<bool>(mFilter))
    {
        return objManager.query(*mFilter);
    }
    else
    {
        throw std::runtime_error("Cannot execute query without ids or filter");
    }
}

DestroyCommand::DestroyCommand(const Id& id)
: mId(id)
{

}

ExecutionResult DestroyCommand::execute(ObjectManager& objManager)
{
    objManager.destroy(mId);
    return Success{};
}

}