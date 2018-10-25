#pragma once

#include "ZmqPbChannel.hpp"

class InboxModel
{
public:
   InboxModel(ZmqPbChannel& channel);

   struct Item
   {
      materia::Id id;
      std::string text;
   };

   std::vector<Item> get();
   materia::Id add(const Item& item);
   void replace(const Item& item);
   void erase(const materia::Id& id);
};