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

std::string queryCondition(const std::string& type, const std::string& condition, materia::ICore3& core)
{
   boost::property_tree::ptree query;
   query.put("operation", "query");
   query.put("filter", "IS(" + type + ") AND " + condition);

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

void deleteAll(const std::string& type, materia::ICore3& core)
{
   auto all = queryAll(type, core);
   auto ol = readJson<boost::property_tree::ptree>(all);
    
   for(auto& v : ol.get_child("object_list"))
   {
       boost::property_tree::ptree dlt;
       dlt.put("operation", "destroy");
       dlt.put("id", v.second.get<std::string>("id"));

       core.executeCommandJson(writeJson(dlt));
   }
}
std::optional<boost::property_tree::ptree> query(const std::string& id, materia::ICore3& core)
{
   std::string query = "{\"operation\": \"query\", \"ids\":[\"" + id + "\"]}";

   auto result = core.executeCommandJson(query);

   auto ol = readJson<boost::property_tree::ptree>(result);
    
   for(auto& v : ol.get_child("object_list"))
   {
      return v.second;
   }

   return std::optional<boost::property_tree::ptree>();
}

void set(const std::string& id, const int value, materia::ICore3& core)
{
   if(query(id, core))
   {
      boost::property_tree::ptree modify;
      modify.put("operation", "modify");
      modify.put("id", id);
      modify.put("params.value", value);

      core.executeCommandJson(writeJson(modify));
   }
   else
   {
      boost::property_tree::ptree create;
      create.put("operation", "create");
      create.put("typename", "variable");
      create.put("defined_id", id);
      create.put("params.value", value);

      core.executeCommandJson(writeJson(create));
   }
}

void createConnection(materia::ICore3& core, const std::string& a, const std::string& b, const std::string& connectionType)
{
      boost::property_tree::ptree create;
      create.put("operation", "create");
      create.put("typename", "connection");
      create.put("params.A", a);
      create.put("params.B", b);
      create.put("params.type", connectionType);

      core.executeCommandJson(writeJson(create));
}

void putArray(boost::property_tree::ptree& ptree, const std::string& fname, const std::vector<std::string>& values)
{
    boost::property_tree::ptree subTree;

    for(auto x : values)
    {
        boost::property_tree::ptree curCh;
        curCh.put("", x);

        subTree.push_back(std::make_pair("", curCh));
    }

    ptree.add_child(fname, subTree);
}

int queryVar(const std::string& name, materia::ICore3& core)
{
    return query(name, core)->get<int>("value");
}
