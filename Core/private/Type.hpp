#pragma once

#include <ctime>
#include <cstdint>
#include <string>
#include <vector>
#include <boost/date_time/gregorian/gregorian.hpp>

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
    Period,
    Money,
    Money_v2,
    Reference,
    Choice
};

struct Time
{
    std::time_t value;
};

struct Money
{
    std::string currency;
    int base;
    int coins;
};

struct Period
{
    Period()
        : days(0)
        , months(0)
        , years(0)
    {
    }

    boost::gregorian::date_duration days;
    boost::gregorian::months months;
    boost::gregorian::years years;
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
struct TypeTraits<Type::Period>
{
   using Class = Period;
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
struct TypeTraits<Type::Money_v2>
{
   using Class = Money;
};

template<>
struct TypeTraits<Type::Reference>
{
   using Class = std::string;
};

std::string to_string(const Type t);

}
