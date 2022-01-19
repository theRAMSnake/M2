#include "Type.hpp"
#include <stdexcept>

namespace materia
{

std::string to_string(const Type t)
{
    switch(t)
    {   
        case Type::Int: return "int";
        case Type::Money_v2: return "money_v2";
        case Type::Double: return "double";
        case Type::Bool: return "bool";
        case Type::String: return "string";
        case Type::Reference: return "reference";
        case Type::StringArray: return "array";
        case Type::Timestamp: return "timestamp";
        case Type::Period: return "period";
        case Type::Choice: return "choice";
    }

    throw std::runtime_error("unknown type");
}

}
