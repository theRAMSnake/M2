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
                mFields.push_back({f.name, false, std::int64_t(0)}); 
                break;

            case Type::Double: 
                mFields.push_back({f.name, false, 0.0}); 
                break;

            case Type::Bool: 
                mFields.push_back({f.name, false, false}); 
                break;

            case Type::String: 
            case Type::Reference:
                mFields.push_back({f.name, false, ""}); 
                break;

            case Type::StringArray: 
                mFields.push_back({f.name, false, std::vector<std::string>{}}); 
                break;

            case Type::Timestamp: 
                mFields.push_back({f.name, false, std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())}); 
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

std::vector<std::pair<std::string, Object>>::const_iterator Object::children_begin() const
{
    return mChildren.begin();
}

std::vector<std::pair<std::string, Object>>::const_iterator Object::children_end() const
{
    return mChildren.end();
}

void Object::setChild(const std::string& tag, const Object& child)
{
    mChildren[tag] = child;
}

void Object::setChildren(const std::string& tag, const std::vector<ObjectPtr>& children)
{
    std::vector<Object> copies;
    for(auto& o : children)
    {
        copies.push_back(*o);
    }

    mChildren[tag] = copies;
}

void Object::setChildren(const std::string& tag, const std::vector<Object>& children)
{
    mChildren[tag] = children;
}

Object::Object(Object&& other) noexcept
: mTypeDef(std::move(other.mTypeDef))
, mId(std::move(other.mId))
, mFields(std::move(other.mFields))
, mChildren(std::move(other.mChildren))
{
}

Field::operator bool() const
{
    return get<Type::Bool>();
}

Id Field::toId() const
{
    return Id(get<Type::String>());
}

bool Field::isReadonly() const
{
    return mReadonly;
}

}