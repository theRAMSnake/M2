#pragma once

#include <Common/Id.hpp>
#include "ZmqPbChannel.hpp"

class CalendarModel
{
public:
   CalendarModel(ZmqPbChannel& channel);

   struct Item
   {
      materia::Id id;
      std::string text;
      std::time_t timestamp;
   };

   std::vector<Item> query(const std::time_t from, const std::time_t to);
   std::vector<Item> next(const std::time_t from, const int limit);
   materia::Id add(const Item& item);
   void replace(const Item& item);
   void erase(const materia::Id& id);
};