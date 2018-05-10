#pragma once


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

private:
   std::string mGuid;
};

}