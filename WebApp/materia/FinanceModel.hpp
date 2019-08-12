#pragma once

#include <Common/Id.hpp>
#include <messages/finance.pb.h>
#include "ZmqPbChannel.hpp"
#include "MateriaServiceProxy.hpp"

class FinanceModel
{
public:
   struct Category
   {
      materia::Id id;
      std::string name;
   };

   FinanceModel(ZmqPbChannel& channel);

   std::vector<Category> getCategories();
   void renameCategory(const materia::Id& id, const std::string& newName);
   void eraseCategory(const materia::Id& id);
   materia::Id addCategory(const std::string& name);

private:
   MateriaServiceProxy<finance::FinanceService> mService;
};