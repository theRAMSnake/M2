#pragma once

#include "MateriaServiceProxy.hpp"
#include "../IActions.hpp"
#include "messages/actions.pb.h"

namespace materia
{

class Actions : public IActions
{
public:
   Actions(materia::ZmqPbChannel& channel);

   std::vector<ActionItem> getRootItems() override;
   std::vector<ActionItem> getChildren(const Id& id) override;
   bool deleteItem(const Id& id) override;
   bool replaceItem(const ActionItem& item) override;
   Id insertItem(const ActionItem& item) override;

   void clear() override;

private:
   MateriaServiceProxy<actions::ActionsService> mProxy;
};

}