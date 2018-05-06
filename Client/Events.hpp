#pragma once

#include "MateriaServiceProxy.hpp"
#include "messages/events.pb.h"
#include <boost/date_time/posix_time/posix_time.hpp>
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
   boost::posix_time::ptime timestamp;
};

struct ContainerUpdatedEvent : public Event
{
   string containerName;
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

class Events
{
public:
   Events(materia::ZmqPbChannel& channel);

   void getEvents(const boost::posix_time::ptime from, IEventHandler& handler);

   template<class TEvent>
   void putEvent(const TEvent& newEvent)
   {
      auto ev = createRawEvent(newEvent);
      common::OperationResultMessage result;
      mProxy.getService().PutEvent(nullptr, &ev, &result, nullptr);
   }

private:
   events::EventInfo createRawEvent(const Event& ev);
   events::EventInfo createRawEvent(const ContainerUpdatedEvent& ev);
   events::EventInfo createRawEvent(const IdEvent& ev);

   MateriaServiceProxy<events::EventsService> mProxy;
};

}