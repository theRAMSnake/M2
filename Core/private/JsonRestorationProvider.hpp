#pragma once
#include "TypeHandler.hpp"
#include <boost/property_tree/ptree.hpp>

namespace materia
{

class JsonRestorationProvider : public IValueProvider
{
public:
    JsonRestorationProvider(const std::string& json);
    void populate(Object& obj) const override;

private:
    boost::property_tree::ptree mImpl;
};

}