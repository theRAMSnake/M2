#pragma once
#include <variant>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include "TraitSystem.hpp"
#include "ObjectManager.hpp"

namespace materia
{

struct Success
{

};
struct Error
{
    std::string error;
};

using ObjectList = std::vector<Params>;
using ExecutionResult = std::variant<Success, ObjectList, std::string, Id>;

class ObjectManager;
class Command
{
public:
    virtual ExecutionResult execute(ObjectManager& objManager) = 0;
    virtual ~Command() {}
};

class CreateCommand : public Command
{
public:
    CreateCommand(const std::vector<std::string>& traits, const Id id, const Params& params);
    ExecutionResult execute(ObjectManager& objManager) override;

private:
    const std::vector<std::string> mTraits;
    const Id mId;
    const Params mParams;
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

class ModifyCommand : public Command
{
public:
    ModifyCommand(const Id& id, const Params& params);
    ExecutionResult execute(ObjectManager& objManager) override;

private:
    const Id mId;
    const Params mParams;
};

class CallCommand : public Command
{
public:
    CallCommand(const std::string& name, const Params& params){}
    ExecutionResult execute(ObjectManager& objManager) override{return "";}
};

class SearchCommand : public Command
{
public:
    SearchCommand(const std::string& phrase){}
    ExecutionResult execute(ObjectManager& objManager) override{return "";}
};

}