#pragma once
#include "TypeSystem.hpp"
#include "Common/Id.hpp"
#include <memory>
#include <map>
#include <variant>
#include <exception>
#include <fmt/format.h>

namespace std
{

const std::string& to_string(const std::string& src);
std::string to_string(const materia::Time& src);
std::string to_string(const std::vector<std::string>& src);

}

namespace materia
{

class Object;
//Design: if value is convertible to another type without loss - it would be converted.
//Can be any materia primitive
class Field
{
public:
    template<class T>
    void operator= (const T& v)
    {
        if(mReadonly)
        {
            throw std::runtime_error("Assigning to readonly field");
        }

        if(mDef)
        {
            mValue = typecastDynamic(mDef->type, v);
        }
        else
        {
            if constexpr (std::is_same<T, std::vector<std::string>>::value)
                mValue = v;
            else
                mValue = std::to_string(v);
        }
    }

    template<Type T>
    const typename TypeTraits<T>::Class get() const
    {
        typename TypeTraits<T>::Class result;

        std::visit([&result](auto&& arg)
        {
            result = typecast<T, decltype(arg)>(arg);
        }, mValue);

        return result;
    }

    //Simplifiers
    operator bool() const;
    Id toId() const;

    bool isReadonly() const;
    Type getType() const;
    std::string getName() const;

private:
    friend class Object;
    using ValueHolder = std::variant<
        std::int64_t, 
        double,
        bool,
        Time,
        std::string,
        std::vector<std::string>
        >;

    Field(const FieldDef& f, const bool readonly, const ValueHolder& value);
    Field(const std::string& name, const bool readonly, const ValueHolder& value);

    template<class T>
    static ValueHolder typecastDynamic(const Type& dest, const T& val)
    {
        switch (dest)
        {
        case Type::Bool:
            //Nothing
            if constexpr (std::is_same<T, bool>::value)
                return val;
            break;

        case Type::Int:
        case Type::Option:
        case Type::Money:
            //Any integral type
            if constexpr (std::is_integral<T>::value)
                return static_cast<std::int64_t>(val);
            break;

        case Type::Double:
            //Any integral type, float and double
            if constexpr (std::is_integral<T>::value || std::is_same<T, double>::value || std::is_same<T, float>::value)
                return static_cast<double>(val);
            break;

        case Type::String:
        case Type::Reference:
            //Anything
            return std::to_string(val);

        case Type::StringArray:
            //Nothing
            if constexpr (std::is_same<T, std::vector<std::string>>::value)
                return val;
            break;

        case Type::Timestamp:
            //Nothing
            if constexpr (std::is_same<T, Time>::value)
                return val;
            break;
        }

        throw std::runtime_error(fmt::format("cannot cast {} to {}", std::to_string(val), to_string(dest)));
    }

    template<Type TDest, class Y>
    static typename TypeTraits<TDest>::Class typecast(const Y& val)
    {
        if constexpr (std::is_same<typename TypeTraits<TDest>::Class, Y>::value)
        {
            return val;
        }

        return std::get<typename TypeTraits<TDest>::Class>(typecastDynamic(TDest, val));
    }

    std::string mName;
    bool mReadonly;
    std::optional<FieldDef> mDef;
    ValueHolder mValue;
};

class Object
{
public:
    Object(const TypeDef& type, const Id id);
    Object(const Object& other);
    Object(Object&& other) noexcept;

    Field& operator [] (const std::string& name);
    const Field& operator [] (const std::string& name) const;

    std::vector<Field>::const_iterator begin() const;
    std::vector<Field>::const_iterator end() const;
    std::vector<std::pair<std::string, Object>>::const_iterator children_begin() const;
    std::vector<std::pair<std::string, Object>>::const_iterator children_end() const;

    void setChildren(const std::string& tag, const std::vector<std::shared_ptr<Object>>& children);
    void setChildren(const std::string& tag, const std::vector<Object>& children);
    void setChild(const std::string& tag, const Object& child);

    TypeDef getType() const;
    Id getId() const;

    void clear();

private:
    void init();

    TypeDef mTypeDef;
    Id mId;
    std::vector<Field> mFields;
    using ChildrenHolder = std::variant<Object, std::vector<Object>>;
    std::map<std::string, ChildrenHolder> mChildren;
};

using ObjectPtr = std::shared_ptr<Object>;

template<>
std::vector<ObjectPtr>::iterator find_by_id(std::vector<ObjectPtr>::iterator beg, std::vector<ObjectPtr>::iterator end, const Id id);

}