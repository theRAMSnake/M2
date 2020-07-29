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

Object::Object(const TypeDef& type, const Id id)
: mTypeDef(type)
, mId(id)
{
    init();   
}

void Object::init()
{
    mFields.push_back({"id", true, {mId.getGuid()}});
    mFields.push_back({"typename", true, {mTypeDef.name}});

    for(auto f : mTypeDef.fields)
    {
        switch(f.type)
        {   
            case Type::Int: 
            case Type::Money: 
            case Type::Option:
                mFields.push_back({f, false, std::int64_t(0)}); 
                break;

            case Type::Double: 
                mFields.push_back({f, false, 0.0}); 
                break;

            case Type::Bool: 
                mFields.push_back({f, false, false}); 
                break;

            case Type::String: 
            case Type::Reference:
                mFields.push_back({f, false, ""}); 
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

Object::Object(const Object& other)
: mTypeDef(other.mTypeDef)
, mId(other.mId)
, mFields(other.mFields)
, mChildren(other.mChildren)
{

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
        mFields.push_back({name, false, ""}); 
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
    return mDef ? mDef->type : Type::String;
}

TypeDef Object::getType() const
{
    return mTypeDef;
}

template<>
std::vector<ObjectPtr>::iterator find_by_id(std::vector<ObjectPtr>::iterator beg, std::vector<ObjectPtr>::iterator end, const Id id)
{
    return std::find_if(beg, end, [&](auto x)->bool {return x->getId() == id;});
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
    mChildren[tag] = std::make_shared<Object>(child);
}

void Object::setChildren(const std::string& tag, const std::vector<ObjectPtr>& children)
{
    std::vector<ObjectPtr> copies;
    for(auto& o : children)
    {
        copies.push_back(std::make_shared<Object>(*o));
    }

    mChildren[tag] = copies;
}

Object::Object(Object&& other) noexcept
: mTypeDef(std::move(other.mTypeDef))
, mId(std::move(other.mId))
, mFields(std::move(other.mFields))
, mChildren(std::move(other.mChildren))
{
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
            case Type::Timestamp: p.put(f.getName(), f.get<Type::Timestamp>().value);break;
            case Type::Double: p.put(f.getName(), f.get<Type::Double>());break;
            case Type::Bool: p.put(f.getName(), f.get<Type::Bool>());break;
            case Type::String: p.put(f.getName(), f.get<Type::String>());break;
            case Type::Reference: p.put(f.getName(), f.get<Type::Reference>());break;
            case Type::Option: p.put(f.getName(), f.get<Type::Option>());break;
            case Type::StringArray: putArray(p, f.getName(), f.get<Type::StringArray>());break;
            default: throw std::runtime_error("Unknown type"); 
        }
    }

    //Put children
    //std::cout << o.mChildren.size() << std::endl;
    for(auto iter = o.mChildren.begin(); iter != o.mChildren.end(); ++iter)
    {
        if(std::holds_alternative<ObjectPtr>(iter->second))
        {
            boost::property_tree::ptree sub;
            //std::cout << getId() << " " << std::get<ObjectPtr>(iter->second)->getId()<< std::endl;
            fillObject(sub, *std::get<ObjectPtr>(iter->second));

            p.put_child(iter->first, sub);
        }
        else
        {
            boost::property_tree::ptree subTree;

            for(auto& x : std::get<1>(iter->second))
            {
                boost::property_tree::ptree curCh;
                fillObject(curCh, *x);

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

}