#pragma once
#include <string>
#include <optional>
#include <Core/ICore3.hpp>
#include <boost/property_tree/ptree.hpp>

bool expectId(const std::string& responce);
bool expectError(const std::string& responce);
std::size_t count(const std::string& responce);
std::string queryAll(const std::string& type, materia::ICore3& core);
void deleteAll(const std::string& type, materia::ICore3& core);
std::string queryCondition(const std::string& type, const std::string& condition, materia::ICore3& core);
std::optional<boost::property_tree::ptree> query(const std::string& id, materia::ICore3& core);
boost::property_tree::ptree queryFirst(const std::string& type, materia::ICore3& core);
void set(const std::string& id, const int value, materia::ICore3& core);
void putArray(boost::property_tree::ptree& ptree, const std::string& fname, const std::vector<std::string>& values);
int queryVar(const std::string& name, materia::ICore3& core);
