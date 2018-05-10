#pragma once

#include "Id.hpp"

namespace materia
{

enum class EventType
{
   HistoryStarted,
   ActionsUpdated,
   ContainerUpdated,
   CalendarUpdated,
   GoalUpdated,
   AffinitiesUpdated,
   MeasurementUpdated
};

struct Event
{
   EventType type;
   std::time_t timestamp;
};

struct ContainerUpdatedEvent : public Event
{
   std::string containerName;
};

struct IdEvent : public Event
{
   materia::Id id;
};

class IEventHandler
{
public:
   virtual void onGenericEvent(const Event& event) = 0;
   virtual void onContainerUpdated(const ContainerUpdatedEvent& event) = 0;
   virtual void onIdEvent(const IdEvent& event) = 0;

   virtual ~IEventHandler() {}
};

class IEvents
{
public:

   virtual void getEvents(const std::time_t from, IEventHandler& handler) = 0;

   virtual void putEvent(const Event& newEvent) = 0;
   virtual void putEvent(const ContainerUpdatedEvent& newEvent) = 0;
   virtual void putEvent(const IdEvent& newEvent) = 0;

   virtual ~IEvents(){}
};

}