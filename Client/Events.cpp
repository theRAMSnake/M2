#include "Events.hpp"

namespace materia
{

Events::Events(materia::ZmqPbChannel& channel)
: mProxy(channel)
{

}

EventType fromProto(const events::EventType src)
{
   switch(src)
   {
      case events::HistoryStarted:
         return EventType::HistoryStarted;

      case events::ActionsUpdated:
         return EventType::ActionsUpdated;

      case events::CalendarUpdated:
         return EventType::CalendarUpdated;

      case events::ContainerUpdated:
         return EventType::ContainerUpdated;

      case events::GoalUpdated:
         return EventType::GoalUpdated;

      case events::AffinitiesUpdated:
         return EventType::AffinitiesUpdated;

      case events::MeasurementUpdated:
         return EventType::MeasurementUpdated;

      default: 
         throw -1;
   }
}

void Events::getEvents(const boost::posix_time::ptime from, IEventHandler& handler)
{
   events::Events evs;
   auto from_Proto = boost::posix_time:: to_time_t(from);

   events::GetEventsRequest req;
   req.set_timestampfrom(from_Proto);

   mProxy.getService().GetEvents(nullptr, &req, &evs, nullptr);

   for(auto x : evs.items())
   {
      auto time = boost::posix_time::from_time_t(x.timestamp());
      if(x.type() == events::ContainerUpdated)
      {
         materia::ContainerUpdatedEvent ev;
         ev.type = EventType::ContainerUpdated;
         ev.timestamp = time;
         ev.containerName = x.container_name();

         handler.onContainerUpdated(ev);
      }
      else if(x.type() == events::GoalUpdated ||
         x.type() == events::MeasurementUpdated)
      {
         materia::IdEvent ev;
         ev.type = fromProto(x.type());
         ev.timestamp = time;
         ev.id = x.id();

         handler.onIdEvent(ev);
      }
      else
      {
         auto evType = fromProto(x.type());
         handler.onGenericEvent({evType, time});
      }
   }
}

events::EventType toProto(const EventType src)
{
   switch(src)
   {
      case EventType::HistoryStarted:
         return events::HistoryStarted;

      case EventType::ActionsUpdated:
         return events::ActionsUpdated;

      case EventType::CalendarUpdated:
         return events::CalendarUpdated;

      case EventType::ContainerUpdated:
         return events::ContainerUpdated;

      case EventType::GoalUpdated:
         return events::GoalUpdated;

      case EventType::AffinitiesUpdated:
         return events::AffinitiesUpdated;

      case EventType::MeasurementUpdated:
         return events::MeasurementUpdated;

      default:
         throw -1;
   }
}

events::EventInfo Events::createRawEvent(const Event& ev)
{
   events::EventInfo nfo;

   nfo.set_type(toProto(ev.type));
   nfo.set_timestamp(boost::posix_time:: to_time_t(ev.timestamp));

   return nfo;
}

events::EventInfo Events::createRawEvent(const ContainerUpdatedEvent& ev)
{
   events::EventInfo nfo;

   nfo.set_type(toProto(ev.type));
   nfo.set_timestamp(boost::posix_time:: to_time_t(ev.timestamp));
   nfo.set_container_name(ev.containerName);

   return nfo;
}

events::EventInfo Events::createRawEvent(const IdEvent& ev)
{
   events::EventInfo nfo;

   nfo.set_type(toProto(ev.type));
   nfo.set_timestamp(boost::posix_time:: to_time_t(ev.timestamp));
   nfo.mutable_id()->CopyFrom(ev.id.toProtoId());

   return nfo;
}

}