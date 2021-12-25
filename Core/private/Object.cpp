#include "Object.hpp"
#include "JsonSerializer.hpp"
#include "fmt/format.h"
#include <iostream>
#include <chrono>

namespace std
{

const std::string& to_string(const std::string& src)
{
    return src;
}

std::string to_string(const materia::Money& src)
{
    return fmt::format("{}.{:0>2}{}", src.base, src.coins, src.currency);
}

std::string to_string(const materia::Period& src)
{
    std::string result;

    if(src.years.number_of_years() > 0)
    {
        result += std::to_string(src.years.number_of_years().as_number()) + "y";
    }
    if(src.months.number_of_months() > 0)
    {
        result += std::to_string(src.months.number_of_months().as_number()) + "m";
    }
    if(src.days.days() > 0)
    {
        result += std::to_string(src.days.days()) + "d";
    }

    return result;
}

std::string to_string(const materia::Time& src)
{
    return to_string(src.value);
}

std::string to_string(const std::vector<std::string>& src)
{
    return "[...]";
}

}

namespace materia
{

void ensureValidKeyName(const std::string& key)
{
    if(key.find('.') != std::string::npos)
    {
        throw std::runtime_error("Invalid key name: " + key);
    }
}

Object::Object(const TypeDef& type, const Id id)
: mTypeDef(type)
, mId(id)
{
    init();   
}

void Object::init()
{
    using namespace boost::gregorian;
    mFields.push_back({"id", true, {mId.getGuid()}});
    mFields.push_back({"typename", true, {mTypeDef.name}});

    for(auto f : mTypeDef.fields)
    {
        switch(f.type)
        {   
            case Type::Int: 
            case Type::Money: 
                mFields.push_back({f, false, std::int64_t(0)}); 
                break;

            case Type::Double: 
                mFields.push_back({f, false, 0.0}); 
                break;

            case Type::Money_v2: 
                mFields.push_back({f, false, Money{"N", 0, 0}}); 
                break;

            case Type::Period: 
                mFields.push_back({f, false, Period()}); 
                break;

            case Type::Bool: 
                mFields.push_back({f, false, false}); 
                break;

            case Type::String: 
            case Type::Reference:
                mFields.push_back({f, false, ""}); 
                break;

            case Type::Choice:
                mFields.push_back({f, false, f.options[0]}); 
                break;

            case Type::StringArray: 
                mFields.push_back({f, false, std::vector<std::string>{}}); 
                break;

            case Type::Timestamp: 
                mFields.push_back({f, false, Time{std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())}}); 
                break;
        }
    }
}

Field& Object::operator [] (const std::string& name)
{
    auto pos = std::find_if(mFields.begin(), mFields.end(), [name](auto x){return x.mName == name;});
    if(pos != mFields.end())
    {
        return *pos;
    }
    else
    {
        ensureValidKeyName(name);
        mFields.push_back({name, false, std::int64_t(0)}); 
        return mFields.back();
    }
}

const Field& Object::operator [] (const std::string& name) const
{
    auto pos = std::find_if(mFields.begin(), mFields.end(), [name](auto x){return x.mName == name;});
    if(pos != mFields.end())
    {
        return *pos;
    }
    else
    {
        throw std::runtime_error("Field with a name " + name + " not found");
    }
}

Field::Field(const FieldDef& f, const bool readonly, const ValueHolder& value)
: mName(f.name)
, mReadonly(readonly)
, mDef(f)
, mValue(value)
{

}

Field::Field(const std::string& name, const bool readonly, const ValueHolder& value)
: mName(name)
, mReadonly(readonly)
, mValue(value)
{

}

Type Field::getType() const
{
    return mDef ? mDef->type : (std::holds_alternative<std::vector<std::string>>(mValue) ? Type::StringArray : Type::String);
}

TypeDef Object::getType() const
{
    return mTypeDef;
}

template<>
std::vector<Object>::iterator find_by_id(std::vector<Object>::iterator beg, std::vector<Object>::iterator end, const Id id)
{
    return std::find_if(beg, end, [&](auto x)->bool {return x.getId() == id;});
}

void Object::clear()
{
    mFields.clear();
    mChildren.clear();
    init();
}

Id Object::getId() const
{
    return mId;
}

std::vector<Field>::const_iterator Object::begin() const
{
    return mFields.begin();
}

std::vector<Field>::const_iterator Object::end() const
{
    return mFields.end();
}

std::string Field::getName() const
{
    return mName;
}

void Object::setChild(const std::string& tag, const Object& child)
{
    ensureValidKeyName(tag);
    auto [iter, res] = mChildren.insert({tag, child});
    if(!res)
    {
        iter->second = child;
    }
}

void Object::setChildren(const std::string& tag, const std::vector<Object>& children)
{
    ensureValidKeyName(tag);
    auto [iter, res] = mChildren.insert({tag, children});
    if(!res)
    {
        iter->second = children;
    }
}

Id Field::toId() const
{
    return Id(get<Type::String>());
}

bool Field::isReadonly() const
{
    return mReadonly;
}

void putArray(boost::property_tree::ptree& ptree, const std::string& fname, const std::vector<std::string>& values)
{
    boost::property_tree::ptree subTree;

    for(auto x : values)
    {
        boost::property_tree::ptree curCh;
        curCh.put("", x);

        subTree.push_back(std::make_pair("", curCh));
    }

    ptree.add_child(fname, subTree);
}

void Object::fillObject(boost::property_tree::ptree& p, const Object& o)
{
    for(auto f : o)
    {
        switch(f.getType())
        {
            case Type::Int: p.put(f.getName(), f.get<Type::Int>());break;
            case Type::Money: p.put(f.getName(), f.get<Type::Money>());break;
            case Type::Money_v2: p.put(f.getName(), std::to_string(f.get<Type::Money_v2>()));break;
            case Type::Timestamp: p.put(f.getName(), f.get<Type::Timestamp>().value);break;
            case Type::Period: p.put(f.getName(), std::to_string(f.get<Type::Period>()));break;
            case Type::Double: p.put(f.getName(), f.get<Type::Double>());break;
            case Type::Bool: p.put(f.getName(), f.get<Type::Bool>());break;
            case Type::String: p.put(f.getName(), f.get<Type::String>());break;
            case Type::Reference: p.put(f.getName(), f.get<Type::Reference>());break;
            case Type::Choice: p.put(f.getName(), f.get<Type::Choice>()); break;
            case Type::StringArray: putArray(p, f.getName(), f.get<Type::StringArray>());break;
            default: throw std::runtime_error("Unknown type"); 
        }
    }

    //Put children
    for(auto iter = o.mChildren.begin(); iter != o.mChildren.end(); ++iter)
    {
        if(std::holds_alternative<Object>(iter->second))
        {
            boost::property_tree::ptree sub;
            fillObject(sub, std::get<Object>(iter->second));

            p.put_child(iter->first, sub);
        }
        else
        {
            boost::property_tree::ptree subTree;

            for(const auto& x : std::get<1>(iter->second))
            {
                boost::property_tree::ptree curCh;
                fillObject(curCh, x);

                subTree.push_back(std::make_pair("", curCh));
            }

            p.add_child(iter->first, subTree);
        }
    }
}

std::string Object::toJson() const
{
    boost::property_tree::ptree p;

    fillObject(p, *this);
    
    return writeJson(p);
}

const Object& Object::getChild(const std::string& tag) const
{
    auto pos = mChildren.find(tag);
    if(pos == mChildren.end())
    {
        throw std::runtime_error("No such children: " + tag);
    }

    return std::get<0>(pos->second);
}

std::vector<Object> Object::getChildren() const
{
    std::vector<Object> result;

    for(auto x : mChildren)
    {
        result.push_back(std::get<0>(x.second));
    }

    return result;
}

const bool Object::contains(const std::string& fieldName) const
{
    return std::find_if(mFields.begin(), mFields.end(), [fieldName](auto f){return f.getName() == fieldName;}) != mFields.end() ||
       mChildren.contains(fieldName); 
}

void Object::clear(const std::string& fieldName)
{
    if(contains(fieldName))
    {
        auto pos = std::find_if(mFields.begin(), mFields.end(), [fieldName](auto f){return f.getName() == fieldName;});
        if(pos->mReadonly || pos->mDef)
        {
            throw std::runtime_error("Attempt to clear mandatory or readonly field");
        }
        else
        {
            mFields.erase(pos);
        }
    }
}

}
