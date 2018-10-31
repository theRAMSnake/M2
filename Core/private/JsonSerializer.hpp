#pragma once

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "Common/Id.hpp"

class JsonReader
{
public:
   JsonReader(const std::string& src)
   {
      std::istringstream is (src);
      read_json (is, m_ptree);
   }

   template<class F>
   void read(const std::string& field, F& out)
   {
      out = m_ptree.get<F>(field);
   }

private:
   boost::property_tree::ptree m_ptree;
};

class JsonWriter
{
public:
   template<class F>
   void write(const std::string& field, const F& in)
   {
      m_ptree.put(field, in);
   }

   std::string getResult()
   {
      std::ostringstream buf; 
      write_json (buf, m_ptree, false);
      return buf.str();
   }

private:
   boost::property_tree::ptree m_ptree;
};

template<class T>
class JsonMap
{

};

template<class T>
T readJson(const std::string& str)
{
   JsonReader r(str);
   T t;

   JsonMap<T>::read(r, t);

   return t;
}

template<class T>
std::string writeJson(const T& t)
{
   JsonWriter w;

   JsonMap<T>::write(t, w);

   return w.getResult();
}

#define BIND_JSON1(T, F1) template<> class JsonMap<T> { public: \
   static void read(JsonReader& r, T& t) { r.read(#F1, t.F1); } \
   static void write(const T& t, JsonWriter& w) { w.write(#F1, t.F1); } };

#define BIND_JSON2(T, F1, F2) template<> class JsonMap<T> { public: \
   static void read(JsonReader& r, T& t) { r.read(#F1, t.F1); r.read(#F2, t.F2); } \
   static void write(const T& t, JsonWriter& w) { w.write(#F1, t.F1); w.write(#F2, t.F2); } };

#define BIND_JSON3(T, F1, F2, F3) template<> class JsonMap<T> { public: \
   static void read(JsonReader& r, T& t) { r.read(#F1, t.F1); r.read(#F2, t.F2); r.read(#F3, t.F3); } \
   static void write(const T& t, JsonWriter& w) { w.write(#F1, t.F1); w.write(#F2, t.F2); w.write(#F3, t.F3); } };

#define BIND_JSON5(T, F1, F2, F3, F4, F5) template<> class JsonMap<T> { public: \
   static void read(JsonReader& r, T& t) { r.read(#F1, t.F1); r.read(#F2, t.F2); r.read(#F3, t.F3); r.read(#F4, t.F4); r.read(#F5, t.F5); } \
   static void write(const T& t, JsonWriter& w) { w.write(#F1, t.F1); w.write(#F2, t.F2); w.write(#F3, t.F3); w.write(#F4, t.F4); w.write(#F5, t.F5);} };

template<>
void JsonReader::read<materia::Id>(const std::string& field, materia::Id& out);

template<>
void JsonWriter::write<materia::Id>(const std::string& field, const materia::Id& in);