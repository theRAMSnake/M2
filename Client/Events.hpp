#pragma once

#include "MateriaServiceProxy.hpp"
#include "messages/events.pb.h"
#include <boost/date_time/posix_time/posix_time.hpp>

namespace materia
{

enum class EventType
{
   HistoryStarted,
   ActionsUpdated,
   ContainerUpdated,
   CalendarUpdated
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

class IEventHandler
{
public:
   virtual void onGenericEvent(const Event& event) = 0;
   virtual void onContainerUpdated(const ContainerUpdatedEvent& event) = 0;

   virtual ~IEventHandler() {}
};

class Events
{
public:
   Events(materia::ZmqPbChannel& channel);

   void getEvents(const boost::posix_time::ptime from, IEventHandler& handler);

   template<class TEvent>
   void putEvent(const Event& newEvent)
   {
      auto ev = createRawEvent(newEvent);
      common::OperationResultMessage result;
      mProxy.getService().PutEvent(nullptr, &ev, &result, nullptr);
   }

private:
   events::EventInfo createRawEvent(const Event& ev);
   events::EventInfo createRawEvent(const ContainerUpdatedEvent& ev);

   MateriaServiceProxy<events::EventsService> mProxy;
};

}