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

   struct Event
   {
      materia::Id eventId;
      materia::Id categoryId;
      std::string details;
      std::time_t timestamp;
      unsigned int amountOfEuroCents;
      finance::EventType eventType;
   };

   struct Report
   {
      int balance;
      finance::FinanceStatus status;
   };

   FinanceModel(ZmqPbChannel& channel);

   std::vector<Category> getCategories();
   void renameCategory(const materia::Id& id, const std::string& newName);
   void eraseCategory(const materia::Id& id);
   materia::Id addCategory(const std::string& name);

   std::vector<Event> loadEvents(const std::time_t timestampFrom, const std::time_t timestampTo, finance::EventType type);
   materia::Id addEvent(const Event& ev);
   void deleteEvent(const materia::Id& id);
   void modifyEvent(const Event& ev);

   Report getReport();

private:
   MateriaServiceProxy<finance::FinanceService> mService;
};