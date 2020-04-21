#pragma once
#include <variant>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include "TypeSystem.hpp"
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
    CreateCommand(const TypeDef& type, const Params& params);
    ExecutionResult execute(ObjectManager& objManager) override;

private:
    const TypeDef mType;
    const Params mParams;
};

class DestroyCommand : public Command
{
public:
    DestroyCommand(const TypeDef& type, const Id& id);
    ExecutionResult execute(ObjectManager& objManager) override;

private:
    const TypeDef mType;
    const Id mId;
};

class QueryCommand : public Command
{
public:
    QueryCommand(const TypeDef& type, std::unique_ptr<Filter>& filter, std::optional<std::string> id);
    ExecutionResult execute(ObjectManager& objManager) override;

private:
    const TypeDef mType;
    const std::unique_ptr<Filter> mFilter;
    const Id mId;
};

class ModifyCommand : public Command
{
public:
    ModifyCommand(const TypeDef& type, const Params& params);
    ExecutionResult execute(ObjectManager& objManager) override;

private:
    const TypeDef mType;
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