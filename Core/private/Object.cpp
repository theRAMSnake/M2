#include "Object.hpp"
#include "JsonSerializer.hpp"
#include "fmt/format.h"
#include <iostream>

namespace materia
{

Object::Object(const TypeDef& type, const Id id)
: mTypeDef(type)
{
    (*this)["id"] = id;
    (*this)["typename"] = mTypeDef.name;

    for(auto f : mTypeDef.fields)
    {
        auto p = (*this)[f.name];
        switch(f.type)
        {   
            case Type::Int: p = 0;break;
            case Type::Double: p = 0.0;break;
            case Type::Bool: p = false;break;
            case Type::String: p = std::string();break;
            case Type::Array: p = std::vector<std::string>();break;
            case Type::Timestamp: p = Time{0}; break;
            case Type::Option: p = 0; break;
        }
    }
}

FieldProxy Object::operator [] (const std::string& name)
{
    auto pos = std::find_if(mTypeDef.fields.begin(), mTypeDef.fields.end(), [name](auto x){return x.name == name;});
    if(pos != mTypeDef.fields.end())
    {
        return FieldProxy(*pos, mImpl);
    }
    else
    {
        return FieldProxy(name, mImpl);
    }
}

const FieldProxy Object::operator [] (const std::string& name) const
{
    auto pos = std::find_if(mTypeDef.fields.begin(), mTypeDef.fields.end(), [name](auto x){return x.name == name;});
    if(pos != mTypeDef.fields.end())
    {
        return FieldProxy(*pos, mImpl);
    }
    else
    {
        return FieldProxy(name, mImpl);
    }
}

std::string Object::toJson() const
{
    return writeJson(mImpl);
}

FieldProxy::FieldProxy(const FieldDef& f, boost::property_tree::ptree& impl)
: mName(f.name)
, mDef(f)
, mType(f.type)
, mImpl(impl)
{

}

FieldProxy::FieldProxy(const std::string& name, boost::property_tree::ptree& impl)
: mName(name)
, mImpl(impl)
{

}

void FieldProxy::operator= (const bool v)
{
    if(mType)
    {
        if(mType == Type::Bool)
        {
            mImpl.put(mName, v);
        }
        else if(mType == Type::String)
        {
            mImpl.put(mName, v ? "true" : "false");
        }
        else
        {
            throw std::runtime_error(fmt::format("Cannot assign bool to {}", mName));
        }
    }
    else
    {
        mImpl.put(mName, v);
    }
}

void FieldProxy::operator= (const Time v)
{
    if(mType)
    {
        if(mType == Type::Timestamp)
        {
            mImpl.put(mName, v.value);
        }
        else
        {
            throw std::runtime_error(fmt::format("Cannot assign timestamp to {}", mName));
        }
    }
    else
    {
        mImpl.put(mName, v.value);
    }
}

void FieldProxy::operator= (const int v)
{
    if(mType)
    {
        if(mType == Type::Double || mType == Type::Int || mType == Type::String || mType == Type::Timestamp)
        {
            mImpl.put(mName, v);
        }
        else if(mType == Type::Option)
        {
            if(v < static_cast<int>(mDef->options.size()) && v >= 0)
            {
                mImpl.put(mName, v);
            }
            else
            {
                throw std::runtime_error(fmt::format("Cannot assign int to {}, value out of range: {}", mName, v));    
            }
        }
        else
        {
            throw std::runtime_error(fmt::format("Cannot assign int to {}", mName));
        }
    }
    else
    {
        mImpl.put(mName, v);
    }
}

void FieldProxy::operator= (const double v)
{
    if(mType)
    {
        if(mType == Type::Double || mType == Type::String)
        {
            mImpl.put(mName, v);
        }
        else
        {
            throw std::runtime_error(fmt::format("Cannot assign double to {}", mName));
        }
    }
    else
    {
        mImpl.put(mName, v);
    }
}

void FieldProxy::operator= (const std::string& v)
{
    if(mType)
    {
        if(mType == Type::String)
        {
            mImpl.put(mName, v);
        }
        else if(mType == Type::Int || mType == Type::Timestamp)
        {
            mImpl.put(mName, boost::lexical_cast<int>(v));
        }
        else if(mType == Type::Option)
        {
            auto s = boost::lexical_cast<int>(v);
            if(s < static_cast<int>(mDef->options.size()) && s >= 0)
            {
                mImpl.put(mName, s);
            }
            else
            {
                throw std::runtime_error(fmt::format("Cannot assign int to {}, value out of range: {}", mName, s));    
            }
        }
        else if(mType == Type::Double)
        {
            mImpl.put(mName, boost::lexical_cast<double>(v));
        }
        else if(mType == Type::Bool)
        {
            mImpl.put(mName, boost::lexical_cast<bool>(v));
        }
        else
        {
            throw std::runtime_error(fmt::format("Cannot assign string to {}", mName));
        }
    }
    else
    {
        mImpl.put(mName, v);
    }
}

void FieldProxy::operator= (const std::vector<std::string>& v)
{
    if(mType && *mType != Type::Array)
    {
        throw std::runtime_error(fmt::format("Cannot assign array to {}", mName));
    }

    boost::property_tree::ptree subTree;
    for(auto& t: v)
    {
        boost::property_tree::ptree p;
        p.put("", t);
        subTree.push_back({"", p});
    }

    mImpl.put_child(mName, subTree);
}

void FieldProxy::operator= (const std::vector<Id>& v)
{
    if(mType && *mType != Type::Array)
    {
        throw std::runtime_error(fmt::format("Cannot assign array to {}", mName));
    }

    boost::property_tree::ptree subTree;
    for(auto& t: v)
    {
        boost::property_tree::ptree p;
        p.put("", t.getGuid());
        subTree.push_back({"", p});
    }

    mImpl.put_child(mName, subTree);
}

void FieldProxy::operator= (const std::vector<std::shared_ptr<Object>>& v)
{
    if(mType)
    {
        throw std::runtime_error(fmt::format("Cannot assign object array to {}", mName));
    }

    boost::property_tree::ptree subTree;
    for(auto& t: v)
    {
        boost::property_tree::ptree p = readJson<boost::property_tree::ptree>(t->toJson());
        subTree.push_back({"", p});
    }

    mImpl.put_child(mName, subTree);
}

FieldProxy::operator Id() const
{
    if(mType && *mType != Type::String)
    {
        throw std::runtime_error(fmt::format("Cannot get id from {}", mName));
    }

    return Id(mImpl.get<std::string>(mName));
}

FieldProxy::operator Time() const
{
    if(mType && *mType != Type::Timestamp)
    {
        throw std::runtime_error(fmt::format("Cannot get timestamp from {}", mName));
    }

    return Time{mImpl.get<std::time_t>(mName)};
}

FieldProxy::operator std::string() const
{
    if(mType)
    {
        try
        {
            return mImpl.get<std::string>(mName);
        }
        catch(...)
        {
            throw std::runtime_error(fmt::format("Cannot get string from {}", mName));        
        }
    }

    return mImpl.get<std::string>(mName);
}

FieldProxy::operator int() const
{
    if(mType)
    {
        try
        {
            return mImpl.get<int>(mName);
        }
        catch(...)
        {
            throw std::runtime_error(fmt::format("Cannot get int from {}", mName));        
        }
    }

    return mImpl.get<int>(mName);
}

FieldProxy::operator bool() const
{
    if(mType)
    {
        try
        {
            return mImpl.get<bool>(mName);
        }
        catch(...)
        {
            throw std::runtime_error(fmt::format("Cannot get bool from {}", mName));        
        }
    }

    return mImpl.get<bool>(mName);
}

FieldProxy::operator double() const
{
    if(mType)
    {
        try
        {
            return mImpl.get<double>(mName);
        }
        catch(...)
        {
            throw std::runtime_error(fmt::format("Cannot get double from {}", mName));        
        }
    }

    return mImpl.get<double>(mName);
}

FieldProxy::operator std::vector<std::string>() const
{
    if(mType && *mType == Type::Array)
    {
        std::vector<std::string> result;

        for(auto x : mImpl.get_child(mName))
        {
            result.push_back(x.second.get_value<std::string>());
        }

        return result;
    }

    throw std::runtime_error(fmt::format("Cannot get array from {}", mName));
}

Type FieldProxy::getType() const
{
    return mType ? *mType : Type::String;
}

std::string Object::getTypeName() const
{
    return mTypeDef.name;
}

TypeDef Object::getType() const
{
    return mTypeDef;
}

}