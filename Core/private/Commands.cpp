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

}