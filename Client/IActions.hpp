#pragma once

#include "Id.hpp"

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

class IActions
{
public:
   virtual std::vector<ActionItem> getRootItems() = 0;
   virtual std::vector<ActionItem> getChildren(const Id& id) = 0;
   virtual bool deleteItem(const Id& id) = 0;
   virtual bool replaceItem(const ActionItem& item) = 0;
   virtual Id insertItem(const ActionItem& item) = 0;
   virtual void clear() = 0;

   virtual ~IActions() {}
};

}