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

Money parseMoney(const std::string& src)
{
    std::istringstream str(src);
    Money result;

    str >> result.base;
    if(!str)
    {
        throw std::runtime_error("Cannot parse money: " + src);
    }

    char dummy;
    str >> dummy;

    if(!str)
    {
        throw std::runtime_error("Cannot parse money: " + src);
    }

    str >> std::setw(2) >> result.coins;
    if(!str)
    {
        throw std::runtime_error("Cannot parse money: " + src);
    }

    str >> std::setw(0) >> result.currency;

    if(!str)
    {
        throw std::runtime_error("Cannot parse money: " + src);
    }

    return result;
}

Period parsePeriod(const std::string& src)
{
    using namespace boost::gregorian;
    Period result;

    std::istringstream str(src);
    while(!str.eof())
    {
        int value = 0;
        str >> value;

        if(!str)
        {
            throw std::runtime_error("Cannot parse period: " + src);
        }

        char symbol = ' ';
        str >> symbol;

        if(!str)
        {
            throw std::runtime_error("Cannot parse period: " + src);
        }
        
        switch(symbol)
        {
            case 'd':
                result.days = boost::gregorian::days(value);
                break;
            case 'm':
                result.months = boost::gregorian::months(value);
                break;
            case 'y':
                result.years = boost::gregorian::years(value);
                break;
            default:
                throw std::runtime_error("Cannot parse period: " + src);
        }
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
            //Array of object deserialization is not supported
            if(c.second.data().empty() && c.second.size() != 0)
            {
                Object subobj({"object"}, Id(c.second.get<std::string>("id")));
                JsonRestorationProvider sub(c.second);
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
