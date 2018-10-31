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