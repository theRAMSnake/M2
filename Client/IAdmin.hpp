#pragma once

#include "Id.hpp"

namespace materia
{

struct ServiceStatus
{
   std::string name;
   bool isAvailable;
};

class IAdmin
{
public:
   virtual std::vector<ServiceStatus> getServiceStatus() = 0;

   virtual ~IAdmin() {}
};

}