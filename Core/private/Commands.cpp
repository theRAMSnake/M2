#include "Commands.hpp"

namespace materia
{

CreateCommand::CreateCommand(const TypeDef& type, const Params& params)
: mType(type)
, mParams(params)
{

}

ExecutionResult CreateCommand::execute(ObjectManager& objManager)
{
    return objManager.create(mType, mParams);
}

ModifyCommand::ModifyCommand(const TypeDef& type, const Params& params)
: mType(type)
, mParams(params)
{

}

ExecutionResult ModifyCommand::execute(ObjectManager& objManager)
{
    objManager.modify(mType, mParams);
    return Success{};
}

QueryCommand::QueryCommand(const TypeDef& type, std::unique_ptr<Filter>& filter)
: mType(type)
, mFilter(std::move(filter))
{

}

ExecutionResult QueryCommand::execute(ObjectManager& objManager)
{
    return static_cast<bool>(mFilter) ? objManager.query(mType, *mFilter) : objManager.query(mType);
}

DestroyCommand::DestroyCommand(const TypeDef& type, const Id& id)
: mType(type)
, mId(id)
{

}

ExecutionResult DestroyCommand::execute(ObjectManager& objManager)
{
    objManager.destroy(mType, mId);
    return Success{};
}

}