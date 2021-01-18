#include "Type.hpp"
#include <stdexcept>

namespace materia
{

std::string to_string(const Type t)
{
    switch(t)
    {   
        case Type::Int: return "int";
        case Type::Money: return "money";
        case Type::Double: return "double";
        case Type::Bool: return "bool";
        case Type::String: return "string";
        case Type::Reference: return "reference";
        case Type::StringArray: return "array";
        case Type::Timestamp: return "timestamp";
        case Type::Option: return "option";
        case Type::Choice: return "choice";
    }

    throw std::runtime_error("unknown type");
}

}
