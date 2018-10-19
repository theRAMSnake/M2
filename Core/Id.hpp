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

private:
   std::string mGuid;
};

template<class TIterator>
TIterator find_by_id(TIterator beg, TIterator end, const Id& id)
{
   return std::find_if(beg, end, [&](auto x)->bool {return x.id == id;});
}

template<class TCollection>
auto find_by_id(TCollection& col, const Id& id) -> decltype(col.begin()) 
{
   return find_by_id(col.begin(), col.end(), id);
}
}