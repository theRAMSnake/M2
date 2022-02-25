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
    std::string currency = "EUR";
    int base = 0;
    int coins = 0;

    template<class T>
    Money& operator /=(const T& modifier)
    {
        fromFullValue(toFullValue() / modifier);

        return *this;
    }

    Money& operator -=(const Money& other)
    {
        if(currency != other.currency)
        {
            throw std::runtime_error("Cannot perform operation on money of different currency");
        }

        fromFullValue(toFullValue() - other.toFullValue());

        return *this;
    }

    Money operator -(const Money& other)
    {
        if(currency != other.currency)
        {
            throw std::runtime_error("Cannot perform operation on money of different currency");
        }

        Money result;
        result.fromFullValue(toFullValue() - other.toFullValue());
        return result;
    }

    double operator /(const Money& other)
    {
        if(currency != other.currency)
        {
            throw std::runtime_error("Cannot perform operation on money of different currency");
        }

        return static_cast<double>(toFullValue()) / other.toFullValue();
    }

    Money& operator +=(const Money& other)
    {
        if(currency != other.currency)
        {
            throw std::runtime_error("Cannot perform operation on money of different currency");
        }

        fromFullValue(toFullValue() + other.toFullValue());

        return *this;
    }

private:
    int toFullValue() const
    {
        return base * 100 + coins;
    }

    void fromFullValue(const int fv)
    {
        base = fv / 100;
        coins = std::abs(fv % 100);
    }
};

struct Period
{
    Period()
        : days(0)
        , months(0)
        , years(0)
    {
    }

    static Period Empty()
    {
        return Period();
    }

    bool operator == (const Period& other) const
    {
        return days == other.days && months == other.months && years == other.years;
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
