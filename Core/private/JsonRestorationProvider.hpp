#pragma once
#include <boost/property_tree/ptree.hpp>
#include "ObjectManager.hpp"

namespace materia
{

Money parseMoney(const std::string& src);
class JsonRestorationProvider : public IValueProvider
{
public:
    JsonRestorationProvider(const std::string& json);
    void populate(Object& obj) const override;

private:
    JsonRestorationProvider(const boost::property_tree::ptree& ptree);
    boost::property_tree::ptree mImpl;
};

}
