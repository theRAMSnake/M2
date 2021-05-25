#include "Commands.hpp"
#include "JsonRestorationProvider.hpp"
#include "rng.hpp"

namespace materia
{

CreateCommand::CreateCommand(const std::optional<Id> id, const std::string& typeName, const std::string& params)
: mTypeName(typeName)
, mId(id)
, mParams(params)
{

}

ExecutionResult CreateCommand::execute(ObjectManager& objManager)
{
    JsonRestorationProvider provider(mParams);
    return objManager.create(mId, mTypeName, provider).getId();
}

ModifyCommand::ModifyCommand(const Id& id, const std::string& params)
: mId(id)
, mParams(params)
{

}

ExecutionResult ModifyCommand::execute(ObjectManager& objManager)
{
    JsonRestorationProvider provider(mParams);
    objManager.modify(mId, provider);
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

ExecutionResult DescribeCommand::execute(ObjectManager& objManager)
{
    return objManager.describe();
}

RandomCommand::RandomCommand(const std::string& typeName)
: mTypeName(typeName)
{

}

ExecutionResult RandomCommand::execute(ObjectManager& objManager)
{
    auto items = objManager.getAll(mTypeName);
    if(!items.empty())
    {
        auto pos = Rng::genChoise(items.size());
        return ObjectList{items[pos]};
    }
    else
    {
        return Error{"No items for " + mTypeName};
    }
}

ExecutionResult BackupCommand::execute(ObjectManager& objManager)
{
    system(("sqlite3 " + mDbName + " \".backup '/materia/materia.back'\"").c_str());
    return Success();
}

BackupCommand::BackupCommand(const std::string& dbName)
: mDbName(dbName)
{

}

bool BackupCommand::isLong() const
{
    return true;
}

bool Command::isLong() const
{
    return false;
}

}