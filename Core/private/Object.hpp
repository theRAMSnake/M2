#pragma once
#include <boost/property_tree/ptree.hpp>
#include "TypeSystem.hpp"
#include "Common/Id.hpp"

namespace materia
{

class Object;
//Design: if value is convertible to another tip without loss - it would be converted.
class FieldProxy
{
public:
    friend class Object;

    void operator= (const bool v);
    void operator= (const int v);
    void operator= (const double v);
    void operator= (const std::string& v);
    void operator= (const std::vector<std::string>& v);
    void operator= (const std::vector<Id>& v);
    void operator= (const std::vector<std::shared_ptr<Object>>& v);

    operator Id() const;
    operator std::string() const;
    operator int() const;
    operator double() const;
    operator bool() const;

    Type getType() const;

private:
    FieldProxy(const FieldDef& f, boost::property_tree::ptree& impl);
    FieldProxy(const std::string& name, boost::property_tree::ptree& impl);

    std::string mName;
    std::optional<Type> mType;
    boost::property_tree::ptree& mImpl;
};

class Object
{
public:
    Object(const TypeDef& type, const Id id);
    FieldProxy operator [] (const std::string& name);
    const FieldProxy operator [] (const std::string& name) const;

    std::string toJson() const;
    std::string getTypeName() const;
    TypeDef getType() const;

private:
    TypeDef mTypeDef;
    mutable boost::property_tree::ptree mImpl;
};

}