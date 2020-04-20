#include "JsonSerializer.hpp"

template<>
void JsonReader::read<materia::Id>(const std::string& field, materia::Id& out)
{
   out = m_ptree.get<std::string>(field);
}

template<>
void JsonWriter::write<materia::Id>(const std::string& field, const materia::Id& in)
{
   m_ptree.put(field, in.getGuid());
}

void JsonMap<bool>::read(JsonReader& r, bool& t)
{
   r.read("value", t);
}

void JsonMap<bool>::write(const bool& t, JsonWriter& w)
{
   w.write("value", t);
}

void JsonMap<boost::property_tree::ptree>::read(JsonReader& r, boost::property_tree::ptree& t)
{
   r.read(t);
}

void JsonMap<boost::property_tree::ptree>::write(const boost::property_tree::ptree& t, JsonWriter& w)
{
   w.write(t);
}