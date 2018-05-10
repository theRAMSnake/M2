#pragma once

#include "MateriaServiceProxy.hpp"
#include "messages/events.pb.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include "../IEvents.hpp"

namespace materia
{

class Events : public IEvents
{
public:
   Events(materia::ZmqPbChannel& channel);

   void getEvents(const std::time_t from, IEventHandler& handler) override;

   virtual void putEvent(const Event& newEvent)  override;
   virtual void putEvent(const ContainerUpdatedEvent& newEvent)  override;
   virtual void putEvent(const IdEvent& newEvent)  override;

private:
   template<class TEvent>
   void putEventImpl(const TEvent& newEvent)
   {
      auto ev = createRawEvent(newEvent);
      common::OperationResultMessage result;
      mProxy.getService().PutEvent(nullptr, &ev, &result, nullptr);
   }

   events::EventInfo createRawEvent(const Event& ev);
   events::EventInfo createRawEvent(const ContainerUpdatedEvent& ev);
   events::EventInfo createRawEvent(const IdEvent& ev);

   MateriaServiceProxy<events::EventsService> mProxy;
};

}