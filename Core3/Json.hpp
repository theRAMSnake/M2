#pragma once

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace materia3
{

class Json
{
public:
    Json();
    Json(const std::string& src);

    template<class T>
    void set(const std::string& fname, const T& value)
    {
        mImpl.put(fname, value);
    }

    template<class T>
    T get(const std::string& fname)
    {
        return mImpl.get<T>(fname);
    }

    bool contains(const std::string& fname);

    std::string str() const;

private:
    boost::property_tree::ptree mImpl;
};

}