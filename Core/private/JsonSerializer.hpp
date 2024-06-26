#pragma once

#include <map>
#include <variant>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/lexical_cast.hpp>
#include "Common/Id.hpp"

template<class T>
class JsonMap
{

};

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
      if(m_ptree.get_optional<F>(field))
      {
         out = m_ptree.get<F>(field);
      }
   }

   void read(boost::property_tree::ptree& out)
   {
      out = m_ptree;
   }

   template<class F>
   void read(const std::string& field, std::vector<F>& out)
   {
      auto ch = m_ptree.get_child_optional(field);

      if(ch)
      {
         for(auto x : *ch)
         {
            std::string json = x.second.get_value<std::string>();

            F nextItem;

            JsonReader subReader(json);
            JsonMap<F>::read(subReader, nextItem);

            out.push_back(nextItem);
         }
      }
   }

   template<class K, class V>
   void read(const std::string& field, std::map<K, V>& out)
   {
      std::vector<std::pair<K, V>> items;
      read(field, items);

      for(auto x : items)
      {
         out.insert(x);
      }
   }

   template<class... T>
   void read(const std::string& field, std::variant<T...>& out)
   {
      auto ch = m_ptree.get_child(field);

      for(auto& c : ch)//Should have exactly one child
      {
         JsonReader subReader(c.second.get_value<std::string>());

         auto i = boost::lexical_cast<unsigned int>(c.first);
         //Hack for now, until we have variant<A,B> only
         if(i == 0)
         {
            using tp = typename std::tuple_element<0, std::tuple<T...> >::type;
            tp tmp;
            JsonMap<tp>::read(subReader, tmp);

            out = tmp;
         }
         else if(i == 1)
         {
            using tp = typename std::tuple_element<1, std::tuple<T...> >::type;
            tp tmp;
            JsonMap<tp>::read(subReader, tmp);

            out = tmp;
         }
         else
         {
            throw -1;
         }
      }      
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

   void write(const boost::property_tree::ptree& out)
   {
      m_ptree = out;
   }

   template<class F>
   void write(const std::string& field, const std::vector<F>& in)
   {
      boost::property_tree::ptree subTree;

      for(auto x : in)
      {
         JsonWriter subWriter;
         JsonMap<F>::write(x, subWriter);

         boost::property_tree::ptree curCh;
         curCh.put("", subWriter.getResult());

         subTree.push_back(std::make_pair("", curCh));
      }

      m_ptree.add_child(field, subTree);
   }

   template<class K, class V>
   void write(const std::string& field, const std::map<K, V>& in)
   {
      std::vector<std::pair<K, V>> items(in.size());
      std::copy(in.begin(), in.end(), items.begin());

      write(field, items);
   }

   template<class... T>
   void write(const std::string& field, const std::variant<T...>& out)
   {
      boost::property_tree::ptree subTree;

      std::visit([&](auto&& arg) {
         JsonWriter subWriter;
         JsonMap<std::decay_t<decltype(arg)>>::write(arg, subWriter);

         subTree.put(std::to_string(out.index()), subWriter.getResult());
      }, out);

      m_ptree.add_child(field, subTree);
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

#define BIND_JSON4(T, F1, F2, F3, F4) template<> class JsonMap<T> { public: \
   static void read(JsonReader& r, T& t) { r.read(#F1, t.F1); r.read(#F2, t.F2); r.read(#F3, t.F3); r.read(#F4, t.F4); } \
   static void write(const T& t, JsonWriter& w) { w.write(#F1, t.F1); w.write(#F2, t.F2); w.write(#F3, t.F3); w.write(#F4, t.F4); } };

#define BIND_JSON5(T, F1, F2, F3, F4, F5) template<> class JsonMap<T> { public: \
   static void read(JsonReader& r, T& t) { r.read(#F1, t.F1); r.read(#F2, t.F2); r.read(#F3, t.F3); r.read(#F4, t.F4); r.read(#F5, t.F5); } \
   static void write(const T& t, JsonWriter& w) { w.write(#F1, t.F1); w.write(#F2, t.F2); w.write(#F3, t.F3); w.write(#F4, t.F4); w.write(#F5, t.F5);} };

#define BIND_JSON6(T, F1, F2, F3, F4, F5, F6) template<> class JsonMap<T> { public: \
   static void read(JsonReader& r, T& t) { r.read(#F1, t.F1); r.read(#F2, t.F2); r.read(#F3, t.F3); r.read(#F4, t.F4); r.read(#F5, t.F5); r.read(#F6, t.F6);} \
   static void write(const T& t, JsonWriter& w) { w.write(#F1, t.F1); w.write(#F2, t.F2); w.write(#F3, t.F3); w.write(#F4, t.F4); w.write(#F5, t.F5); w.write(#F6, t.F6);}};

#define BIND_JSON7(T, F1, F2, F3, F4, F5, F6, F7) template<> class JsonMap<T> { public: \
   static void read(JsonReader& r, T& t) { r.read(#F1, t.F1); r.read(#F2, t.F2); r.read(#F3, t.F3); r.read(#F4, t.F4); r.read(#F5, t.F5); r.read(#F6, t.F6); r.read(#F7, t.F7);} \
   static void write(const T& t, JsonWriter& w) { w.write(#F1, t.F1); w.write(#F2, t.F2); w.write(#F3, t.F3); w.write(#F4, t.F4); w.write(#F5, t.F5); w.write(#F6, t.F6); w.write(#F7, t.F7);} };

#define SERIALIZE_AS_INTEGER(T) namespace std { \
   static ostream& operator << (ostream& str, const T& t){ str << static_cast<int>(t); return str; } \
   static istream& operator >> (istream& str, T& t){ int v; str >> v; t = static_cast<T>(v); return str; }}

template<>
void JsonReader::read<materia::Id>(const std::string& field, materia::Id& out);

template<>
void JsonWriter::write<materia::Id>(const std::string& field, const materia::Id& in);

template<> 
class JsonMap<bool> 
{ 
public: 
   static void read(JsonReader& r, bool& t);
   static void write(const bool& t, JsonWriter& w);
};

template<> 
class JsonMap<boost::property_tree::ptree> 
{ 
public: 
   static void read(JsonReader& r, boost::property_tree::ptree& t);
   static void write(const boost::property_tree::ptree& t, JsonWriter& w);
};