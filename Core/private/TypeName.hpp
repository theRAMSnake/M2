#pragma once
#include <string>

template<class T>
struct TypeName
{
   static const char* get()
   {
      return "?";
   }
};

template<>
struct TypeName<int>
{
   static const char* get()
   {
      return "int";
   }
};

template<>
struct TypeName<std::string>
{
   static const char* get()
   {
      return "string";
   }
};

template<>
struct TypeName<double>
{
   static const char* get()
   {
      return "double";
   }
};

template<>
struct TypeName<bool>
{
   static const char* get()
   {
      return "bool";
   }
};