#pragma once
#include <variant>
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
struct ObjectList
{

};

using ArgumentValue = std::variant<int, double, std::string, bool>;
class FilterArgument
{
public:
    virtual ArgumentValue evaluate() const = 0;
};

class BinaryExpression : public FilterArgument
{
public:
    virtual const FilterArgument& getA() const = 0;
    virtual const FilterArgument& getB() const = 0;
};

using Filter = BinaryExpression;
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
    DestroyCommand(const TypeDef& type, const Id& id){}
    ExecutionResult execute(ObjectManager& objManager) override{return "";}
};

class QueryCommand : public Command
{
public:
    QueryCommand(const TypeDef& type, const std::unique_ptr<Filter>& filter){}
    ExecutionResult execute(ObjectManager& objManager) override{return "";}
};

class ModifyCommand : public Command
{
public:
    ModifyCommand(const TypeDef& type, const Params& params){}
    ExecutionResult execute(ObjectManager& objManager) override{return "";}
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