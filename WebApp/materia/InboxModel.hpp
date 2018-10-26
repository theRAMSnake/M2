#pragma once

#include <Common/Id.hpp>
#include <messages/inbox.pb.h>
#include "ZmqPbChannel.hpp"
#include "MateriaServiceProxy.hpp"

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

private:
   MateriaServiceProxy<inbox::InboxService> mService;
};