#include "JsonRestorationProvider.hpp"
#include "JsonSerializer.hpp"

namespace materia
{

JsonRestorationProvider::JsonRestorationProvider(const std::string& json)
{
    mImpl = readJson<boost::property_tree::ptree>(json);
}

JsonRestorationProvider::JsonRestorationProvider(const boost::property_tree::ptree& ptree)
: mImpl(ptree)
{

}

std::vector<std::string> extractArray(const boost::property_tree::ptree& ptree)
{
    std::vector<std::string> result;

    for(auto x : ptree)
    {
        result.push_back(x.second.get_value<std::string>());
    }

    return result;
}

void JsonRestorationProvider::populate(Object& obj) const
{
    auto type = obj.getType();
    for(auto c : mImpl)
    {
        auto& f = obj[c.first];
        if(f.isReadonly())
        {
            continue;
        }

        auto pos = std::find_if(type.fields.begin(), type.fields.end(), [=](auto x){return x.name == c.first;});
        if(pos != type.fields.end())
        {
            switch(pos->type)
            {
                case Type::Int: f = c.second.get_value<int>();break;
                case Type::Money: f = c.second.get_value<int>();break;
                case Type::Timestamp: f = Time{c.second.get_value<std::time_t>()};break;
                case Type::Double: f = c.second.get_value<double>();break;
                case Type::Bool: f = c.second.get_value<bool>();break;
                case Type::String: f = c.second.get_value<std::string>();break;
                case Type::Reference: f = c.second.get_value<std::string>();break;
                case Type::Option: f = c.second.get_value<int>();break;
                case Type::StringArray: f = extractArray(c.second);break;
                default: throw std::runtime_error("Unknown type"); 
            }
        }
        else
        {
            if(c.second.data().empty())
            {
                for(auto cc : c.second)
                {
                    Object subobj({"object"}, Id(cc.second.get<std::string>("id")));
                    JsonRestorationProvider sub(cc.second);
                    sub.populate(subobj);

                    obj.appendChild(c.first, subobj);
                }
            }
            else
            {
                f = c.second.get_value<std::string>();
            }
        }
    }
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

void fillObject(boost::property_tree::ptree& p, const Object& o)
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
    for(auto iter = o.children_begin(); iter != o.children_end(); ++iter)
    {
        boost::property_tree::ptree sub;
        fillObject(sub, iter->second);

        p.put_child(iter->first, sub);
    }
}

std::string toJson(const Object& o)
{
    boost::property_tree::ptree p;

    fillObject(p, o);
    
    return writeJson(p);
}

}