#pragma once

#include <ctime>
#include <cstdint>
#include <string>
#include <vector>

namespace materia
{

enum class Type
{
    Int,
    Double,
    String,
    Bool,
    StringArray,
    Timestamp,
    Option,
    Money,
    Reference,
    Choice
};

struct Time
{
    std::time_t value;
};

template<Type>
struct TypeTraits
{
};

template<>
struct TypeTraits<Type::Int>
{
   using Class = std::int64_t;
};

template<>
struct TypeTraits<Type::Double>
{
   using Class = double;
};

template<>
struct TypeTraits<Type::String>
{
   using Class = std::string;
};

template<>
struct TypeTraits<Type::Bool>
{
   using Class = bool;
};

template<>
struct TypeTraits<Type::StringArray>
{
   using Class = std::vector<std::string>;
};

template<>
struct TypeTraits<Type::Timestamp>
{
   using Class = Time;
};

template<>
struct TypeTraits<Type::Option>
{
   using Class = std::int64_t;
};

template<>
struct TypeTraits<Type::Choice>
{
   using Class = std::string;
};

template<>
struct TypeTraits<Type::Money>
{
   using Class = std::int64_t;
};

template<>
struct TypeTraits<Type::Reference>
{
   using Class = std::string;
};

std::string to_string(const Type t);

}