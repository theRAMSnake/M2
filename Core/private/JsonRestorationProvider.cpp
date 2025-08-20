#include "JsonRestorationProvider.hpp"
#include "JsonSerializer.hpp"
#include <regex>

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

Period parsePeriod(const std::string& src)
{
    using namespace boost::gregorian;
    using namespace std;
    Period result;

    if(src.empty())
    {
        return result;
    }

    regex r("([0-9]+y)*([0-9]+m)*([0-9]+d)*", regex::extended);
    smatch sm;
    if(regex_search(src, sm, r))
    {
        if(sm[1].matched)
        {
            result.years = boost::gregorian::years(stoi(sm[1]));
        }
        if(sm[2].matched)
        {
            result.months = boost::gregorian::months(stoi(sm[2]));
        }
        if(sm[3].matched)
        {
            result.days = boost::gregorian::days(stoi(sm[3]));
        }
    }
    else
    {
        throw std::runtime_error("Cannot parse period: " + src);
    }
        
    return result;
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

bool is_array_of_objects(const boost::property_tree::ptree& node) {
    // Assuming arrays are represented by multiple children with the same key
    // or unnamed nodes. This logic may need to be adjusted based on your actual JSON structure.
    if (node.empty()) {
        return false; // Empty node, not an array
    }

    if (node.size() == 1) {
        return false;
    }

    auto first_child_key = node.begin()->first;
    for (const auto& child : node) {
        if (child.first != first_child_key) {
            return false; // Different keys, not an array
        }
    }
    return true;
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
                case Type::Money_v2: f = parseMoney(c.second.get_value<std::string>());break;
                case Type::Period: f = parsePeriod(c.second.get_value<std::string>());break;
                case Type::Timestamp: f = Time{c.second.get_value<std::time_t>()};break;
                case Type::Double: f = c.second.get_value<double>();break;
                case Type::Bool: f = c.second.get_value<bool>();break;
                case Type::String: f = c.second.get_value<std::string>();break;
                case Type::Reference: f = c.second.get_value<std::string>();break;
                case Type::Choice: f = c.second.get_value<std::string>();break;
                case Type::StringArray: f = extractArray(c.second);break;
                default: throw std::runtime_error("Unknown type"); 
            }
        }
        else
        {
            if(c.second.data().empty() && c.second.size() != 0 && !is_array_of_objects(c.second))
            {
                Object subobj({"object"}, Id(c.second.get<std::string>("id")));
                JsonRestorationProvider sub(c.second);
                sub.populate(subobj);

                obj.setChild(c.first, subobj);
            }
            else if(c.second.data().empty() && c.second.size() != 0 && is_array_of_objects(c.second))
            {
                std::vector<Object> objectArray;
                for (const auto& item : c.second)
                {
                    Object subobj({"object"}, Id(item.second.get<std::string>("id")));
                    JsonRestorationProvider sub(item.second);
                    sub.populate(subobj);
                    objectArray.push_back(subobj);
                }
                obj.setChildren(c.first, objectArray);
            }
            else
            {
                f = c.second.get_value<std::string>();
            }
        }
    }
}

}
