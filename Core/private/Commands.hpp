#pragma once
#include <variant>
#include <vector>
#include <string>
#include "TypeSystem.hpp"
#include "ObjectManager.hpp"
#include "Connections.hpp"

namespace materia
{

struct Success
{

};
struct Error
{
    std::string error;
};

using ObjectList = std::vector<Object>;
using ConnectionsList = std::vector<Connection>;
using ExecutionResult = std::variant<Success, Error, ObjectList, std::pair<ObjectList, ConnectionsList>, std::string, Id, std::size_t>;

class ObjectManager;
class Command
{
public:
    virtual ExecutionResult execute(ObjectManager& objManager) = 0;
    virtual bool isLong() const;
    virtual ~Command() {}
};

class CreateCommand : public Command
{
public:
    CreateCommand(const std::optional<Id> id, const std::string& typeName, const std::string& params);
    ExecutionResult execute(ObjectManager& objManager) override;

private:
    const std::string mTypeName;
    const std::optional<Id> mId;
    const std::string mParams;
};

class DestroyCommand : public Command
{
public:
    DestroyCommand(const Id& id);
    ExecutionResult execute(ObjectManager& objManager) override;

private:
    const Id mId;
};

class QueryCommand : public Command
{
public:
    QueryCommand(std::shared_ptr<Filter>& filter, const std::vector<Id>& ids);
    ExecutionResult execute(ObjectManager& objManager) override;

private:
    const std::vector<Id> mIds;
    const std::shared_ptr<Filter> mFilter;
};

class CountCommand : public Command
{
public:
    CountCommand(std::shared_ptr<Filter>& filter);
    ExecutionResult execute(ObjectManager& objManager) override;

private:
    const std::shared_ptr<Filter> mFilter;
};

class RandomCommand : public Command
{
public:
    RandomCommand(const std::string& typeName);
    ExecutionResult execute(ObjectManager& objManager) override;

private:
    const std::string mTypeName;
};

class ModifyCommand : public Command
{
public:
    ModifyCommand(const Id& id, const std::string& params);
    ExecutionResult execute(ObjectManager& objManager) override;

private:
    const Id mId;
    const std::string mParams;
};

class DescribeCommand : public Command
{
public:
    ExecutionResult execute(ObjectManager& objManager) override;
};

class BackupCommand : public Command
{
public:
    BackupCommand(const std::string& dbName);
    ExecutionResult execute(ObjectManager& objManager) override;
    bool isLong() const override;

private:
    const std::string mDbName;
};

}
