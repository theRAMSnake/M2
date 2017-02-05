#pragma once
#include <string>

namespace materia
{
   
class IMateriaServiceProvider
{
public:
   virtual std::string execute(const std::string& input) = 0;
};
   
}
