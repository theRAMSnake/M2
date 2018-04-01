#pragma once

#include "MateriaServiceProxy.hpp"
#include "Id.hpp"

#include "messages/admin.pb.h"

#include <boost/optional.hpp>

namespace materia
{

struct ServiceStatus
{
   std::string name;
   bool isAvailable;
};

class Admin
{
public:
   Admin(materia::ZmqPbChannel& channel);
   std::vector<ServiceStatus> getServiceStatus();

private:
   std::string mCategory;
   MateriaServiceProxy<admin::AdminService> mProxy;
};

}