#include "Utils.hpp"
#include <boost/property_tree/ptree.hpp>
#include "../Core/private/JsonSerializer.hpp"
#include <iostream>

bool expectError(const std::string& responce)
{
    auto t = readJson<boost::property_tree::ptree>(responce);
    return t.get_optional<std::string>("error").has_value();
}

bool expectId(const std::string& responce)
{
    auto t = readJson<boost::property_tree::ptree>(responce);
    auto r = t.get_optional<std::string>("result_id").has_value();

    if(r)
    {
        return true;
    }
    else
    {
        std::cout << "Expected id but was " << responce << std::endl;
        return false;
    }
}

std::size_t count(const std::string& responce)
{
   auto ol = readJson<boost::property_tree::ptree>(responce);
    
   std::size_t counter = 0;
   for(auto& v : ol.get_child("object_list"))
   {
      (void)v;
      counter++;
   }

   return counter;
}

std::string queryAll(const std::string& type, materia::ICore3& core)
{
   boost::property_tree::ptree query;
   query.put("operation", "query");
   query.put("filter", "IS(" + type + ")");

   return core.executeCommandJson(writeJson(query));
}

boost::property_tree::ptree queryFirst(const std::string& type, materia::ICore3& core)
{
   boost::property_tree::ptree query;
   query.put("operation", "query");
   query.put("filter", "IS(" + type + ")");

   auto ol = readJson<boost::property_tree::ptree>(core.executeCommandJson(writeJson(query)));
    
   for(auto& v : ol.get_child("object_list"))
   {
      return v.second;
   }

   throw std::runtime_error("Empty list loaded");
}