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
            //Array of object deserialization is not supported
            if(c.second.data().empty() && c.second.size() != 0)
            {
                auto& cc = (*c.second.begin());
                Object subobj({"object"}, Id(cc.second.get<std::string>("id")));
                JsonRestorationProvider sub(cc.second);
                sub.populate(subobj);

                obj.setChild(c.first, subobj);
            }
            else
            {
                f = c.second.get_value<std::string>();
            }
        }
    }
}

}