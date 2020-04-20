#pragma once

#include <boost/property_tree/ptree.hpp>
#include <fmt/format.h>

namespace materia
{

template<class T>
T getOrThrow(const boost::property_tree::ptree& src, const std::string& key, const std::string& error)
{
    try
    {
        return src.get<T>(key);
    }
    catch(...)
    {
        throw std::runtime_error(fmt::format("Unable to get {}", key));
    }
}

template<class T, class Y>
Y getOrThrow(const std::map<T, Y>& src, const T& key, const std::string& error)
{
    auto pos = src.find(key);
    if(pos != src.end())
    {
        return pos->second;
    }

    throw std::runtime_error(error);
}

}