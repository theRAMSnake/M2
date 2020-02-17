#pragma once

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace materia3
{

class Json
{
public:
    template<class T>
    void set(const std::string& fname, const T& value)
    {

    }

    std::string str() const
    {
        std::ostringstream str;
        boost::property_tree::json_parser::write_json(str, mImpl);

        return str.str();
    }

private:
    boost::property_tree::ptree mImpl;
};

}