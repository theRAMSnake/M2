#pragma once

#include "MateriaServiceProxy.hpp"
#include "Id.hpp"

#include "messages/actions.pb.h"

#include <boost/optional.hpp>

namespace materia
{

enum class ActionType
{
   Task,
   Group,
   Unknown
};

struct ActionItem
{
   Id id;
   Id parentId;
   std::string title;
   std::string description;
   ActionType type;

   bool operator != (const ActionItem& other) const;
   bool operator == (const ActionItem& other) const;
};

class Actions
{
public:
   Actions(materia::ZmqPbChannel& channel);

   std::vector<ActionItem> getRootItems();
   std::vector<ActionItem> getChildren(const Id& id);
   bool deleteItem(const Id& id);
   bool replaceItem(const ActionItem& item);
   Id insertItem(const ActionItem& item);

private:
   MateriaServiceProxy<actions::ActionsService> mProxy;
};

}