#pragma once

#include <string>
#include <algorithm>

namespace materia
{

class Id
{
public:
   const static Id Invalid;

   Id();
   Id(const std::string& guid);

   bool operator == (const Id& other) const;
   bool operator != (const Id& other) const;
   bool operator < (const Id& other) const;
   
   const std::string& getGuid() const;
   operator std::string() const;

   static Id generate();

private:
   std::string mGuid;
};

template<class TIterator>
TIterator find_by_id(TIterator beg, TIterator end, const Id id)
{
   return std::find_if(beg, end, [&](auto x)->bool {return x.id == id;});
}

template<class TCollection>
auto find_by_id(TCollection& col, const Id id) -> decltype(col.begin()) 
{
   return find_by_id(col.begin(), col.end(), id);
}

template<class TCollection>
bool contains_id(TCollection& col, const Id id)
{
   return find_by_id(col.begin(), col.end(), id) != col.end();
}
}

namespace std
{

std::ostream& operator << (std::ostream& str, const materia::Id& id);
std::istream& operator >> (std::istream& str, materia::Id& id);
std::string to_string(const materia::Id id);

}