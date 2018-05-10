#pragma once

#include "MateriaServiceProxy.hpp"
#include "Id.hpp"
#include "../IActions.hpp"
#include "messages/actions.pb.h"

#include <boost/optional.hpp>

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

private:
   MateriaServiceProxy<actions::ActionsService> mProxy;
};

}