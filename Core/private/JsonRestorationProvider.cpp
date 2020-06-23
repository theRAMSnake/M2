#include "JsonRestorationProvider.hpp"
#include "JsonSerializer.hpp"

namespace materia
{

JsonRestorationProvider::JsonRestorationProvider(const std::string& json)
{
    mImpl = readJson<boost::property_tree::ptree>(json);
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
        auto pos = std::find_if(type.fields.begin(), type.fields.end(), [=](auto x){return x.name == c.first;});
        if(pos != type.fields.end())
        {
            switch(pos->type)
            {
                case Type::Int: obj[c.first] = c.second.get_value<int>();break;
                case Type::Money: obj[c.first] = c.second.get_value<int>();break;
                case Type::Timestamp: obj[c.first] = Time{c.second.get_value<std::time_t>()};break;
                case Type::Double: obj[c.first] = c.second.get_value<double>();break;
                case Type::Bool: obj[c.first] = c.second.get_value<bool>();break;
                case Type::String: obj[c.first] = c.second.get_value<std::string>();break;
                case Type::Reference: obj[c.first] = c.second.get_value<std::string>();break;
                case Type::Option: obj[c.first] = c.second.get_value<int>();break;
                case Type::Array: obj[c.first] = extractArray(c.second);break;
                default: throw std::runtime_error("Unknown type"); 
            }
        }
        else
        {
            obj[c.first] = c.second.get_value<std::string>();
        }
    }
}

}