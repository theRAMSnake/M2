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

    template<class F>
    void set(const std::string& fname, const std::vector<F>& in)
    {
        boost::property_tree::ptree subTree;

        for(auto x : in)
        {
            boost::property_tree::ptree curCh;
            curCh.put("", x);

            subTree.push_back(std::make_pair("", curCh));
        }

        mImpl.add_child(fname, subTree);
    }

    template<class T>
    T get(const std::string& fname) const
    {
        return mImpl.get<T>(fname);
    }

    template<class F>
    void get(const std::string& fname, std::vector<F>& out)
    {
        auto ch = mImpl.get_child(fname);

        for(auto x : ch)
        {
            out.push_back(x.second.get_value<F>());
        }
    }

    bool contains(const std::string& fname) const;

    std::string str() const;

private:
    boost::property_tree::ptree mImpl;
};

}