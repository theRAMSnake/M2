#pragma once

#include "MateriaServiceProxy.hpp"
#include "messages/admin.pb.h"
#include "../IAdmin.hpp"

namespace materia
{

class Admin : public IAdmin
{
public:
   Admin(materia::ZmqPbChannel& channel);
   std::vector<ServiceStatus> getServiceStatus() override;

private:
   MateriaServiceProxy<admin::AdminService> mProxy;
};

}