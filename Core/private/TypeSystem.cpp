#include "TypeSystem.hpp"
#include <fmt/format.h>

namespace materia
{

TypeSystem::TypeSystem()
{
    mTypes.push_back({"object", "objects"});
    mTypes.push_back({"variable", "variables", {{"value", Type::String}}});
    mTypes.push_back({"simple_list", "lists", {{"objects", Type::Array}}});
}

std::optional<TypeDef> TypeSystem::get(const std::string& name) const
{
    auto pos = std::find_if(mTypes.begin(), mTypes.end(), [name](auto x){return x.name == name;});
    
    if(pos != mTypes.end())
    {
        return *pos;
    }

    return std::optional<TypeDef>();
}

std::vector<TypeDef> TypeSystem::get() const
{
    return mTypes;
}

void TypeSystem::add(const TypeDef& newType)
{
    if(get(newType.name))
    {
        throw std::runtime_error("Trying to add type which already exist");
    }

    mTypes.push_back(newType);
}

}